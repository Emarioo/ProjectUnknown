# Script by Emarioo / Dataolsson, Updated 2021-08-20
# Watch Dataolsson on youtube for more information
# If any error occurs please contact Dataolsson on youtube so I can update this script

bl_info = {
"name": "Model Exporter",
"category": "Object",
"author": "Dataolsson",
"description": "Menu and panel for turning blender models, animations and armatures into data files"
}

import bpy
import struct
import math
import os
import shutil
from mathutils import Matrix
from mathutils import Vector

# How To use
#   1. Select a collection with no collection child
#       if you select collection with children then the object you have selected
#       will be loaded as an independent model
#   2. Be in OBJECT mode
#   3. Press Export Models in Model Exporter tab in the 3d viewport
#     (Define your path/directory first and run this script if you don't see the tab)
#
# Notes
#   Supported Interpolation: Beizer, Linear, Constant
#   C at the end of a mesh to make it a collider.
#   Collider Mesh only supports quad polygons
#
# Check out this link if you want to change quaternions to euler rotation:
#   https://blender.stackexchange.com/questions/40711/how-to-convert-quaternions-keyframes-to-euler-ones-in-several-actions

# Change the code below at your own risk!

# Todo
#  If you select a mesh object. A .mesh file should be created.
#  If you select an armature object, an armature should be created.
#  If you select collection you will make a model which conatins mesh, armature, animation...
#  Possibility to export animations even if they aren't stashed.

#  Export curve graphs for keyframes. Customized interpolation.
#  Animations with inverse kinematics. When exporting animation the control and point bones can be ignored.
#  The bones that matter are the ones with weight. Master or root bone is useful though.
#  When exporting, if there is a keyframe two keyframes forward that is different from the original.
#  Then ignore the keyframe after. Especially if a curve has two keyframes with the same value.

# Important
# X = X, Y = Z, Z = -Y

# Used to switch the axis (Y-up instead of Z-up)
axisConvert = Matrix((
(1.0, 0.0, 0.0, 0.0),
(0.0, 0.0, -1.0, 0.0),
(0.0, 1.0, 0.0, 0.0),
(0.0, 0.0, 0.0, 1.0)))
axisConvertInv = axisConvert.invert()

log=["INFO",""]

# Error logger
def Logging(type,msg):
    global log
    if log[0]=='INFO':
        if type=='INFO':
            log[1]=log[1]+msg+", "
        else:
            log[0]=type
            log[1]=msg
    elif log[0]=='WARNING':
        if type=='WARNING':
            log[1]=log[1]+msg+", "
        elif type=='ERROR':
            log[0]=type
            log[1]=msg
    elif log[0]=='ERROR':
        if type=='ERROR':
            log[1]=log[1]+ msg+", "

# File functions
binaryForm=False
class FileWriter():
    def __init__(self,path):
        try:
            if binaryForm:
                self.file=open(path,"wb")
            else:
                self.file=open(path,"w")
            self.error=False
        except FileNotFoundError:
            print("FileWriter error with "+path)
            self.error=True
        
    def writeUChar(self,*value):
        if self.error:
            return
        if binaryForm:
            print(len(value))
            self.file.write(struct.pack("=%dB"%len(value),*value))
        else:
            for i in range(0,len(value)):
                if not i==0:
                    self.file.write(" ")
                self.file.write(str(value[i]))
            self.file.write("\n")
        
    def writeUShort(self,*value):
        if self.error:
            return
        if binaryForm:
            self.file.write(struct.pack("=%dH"%len(value),*value))
        else:
            for i in range(0,len(value)):
                if not i==0:
                    self.file.write(" ")
                self.file.write(str(value[i]))
            self.file.write("\n")
            
    def writeShort(self,*value):
        if self.error:
            return
        if binaryForm:
            self.file.write(struct.pack("=%dh"%len(value),*value))
        else:
            for i in range(0,len(value)):
                if not i==0:
                    self.file.write(" ")
                self.file.write(str(value[i]))
            self.file.write("\n")
        
    def writeFloat(self,*value):
        if self.error:
            return
        if binaryForm:
            self.file.write(struct.pack("=%df"%len(value),*value))
        else:
            for i in range(0,len(value)):
                if not i==0:
                    self.file.write(" ")
                self.file.write(str(value[i]))
            self.file.write("\n")
            
    def writeMatrix(self,value):
        if self.error:
            return
        if binaryForm:
            self.file.write(struct.pack("=16f",*value[0],*value[1],*value[2],*value[3]))
        else:
            for i in range(0,4):
                self.file.write(str(value[i][0])+" "+str(value[i][1])+" "+str(value[i][2])+" "+str(value[i][3])+"\n")
        
    def writeString(self,value):
        if self.error:
            return
        if binaryForm:
            self.file.write(struct.pack("=B",len(value)))
            if len(value)>0:
                self.file.write(bytearray(value,"UTF-8"))
        else:
            self.file.write(value+"\n");
            
    def writeComment(self,value):
        if self.error:
            return
        if not binaryForm:
            self.file.write("# "+value+"\n");
            
    def close(self):
        if self.error:
            return
        self.file.close()
    
def WriteMesh(path,original):
    file = FileWriter(path+original.data.name+original["MESHTYPE"]+".mesh")
    if file.error:
        return
    
    #print(original.data.name)
    
    bpy.ops.object.select_all(action="DESELECT")
    # Make a copy of the object and apply modifiers
    object=original.copy()
    object.data=original.data.copy()
    bpy.context.scene.collection.objects.link(object)
    object.select_set(True)
    bpy.context.view_layer.objects.active=object
    
    #print(bpy.context.object)
    
    if object.modifiers.find("Triangulate")==-1:
        object.modifiers.new("Triangulate",'TRIANGULATE')
        #object.modifiers["Triangulate"].quad_method="SHORTEST_DIAGONAL"
    
    useArmature=False
    if original["MESHTYPE"]=="-B":
        useArmature=True
                                
    for mod in object.modifiers:
        if not mod.type=='ARMATURE':
            bpy.ops.object.modifier_apply(modifier=mod.name)
    
    triC=0
    for poly in object.data.polygons:
        if len(poly.vertices)==4:
            triC=triC+2
        else:
            triC=triC+1
    
    # Acquire Data
    verC=len(object.data.vertices)
    
    # Combine UV and Material index
    uniqueColor=[]
    indexColor=[]
    if not object.data.uv_layers.active==None:
        tempU=[]
        tempI=[]
        for u in object.data.uv_layers.active.data:
            tempI.append(len(tempU))
            tempU.append([u.uv[0],u.uv[1],0]);
            
        for poly in object.data.polygons:
            lo = poly.loop_indices
            for i in range(0,len(lo)):
                tempU[tempI[lo[i]]][2]=poly.material_index
                #print(str(indexColor[lo[i]])+" "+str(poly.material_index))
        #print("--")
        for u in tempU:
            #print(u)
            same = False
            for i in range(0,len(uniqueColor)):
                if uniqueColor[i][0]==u[0] and uniqueColor[i][1]==u[1] and uniqueColor[i][2]==u[2]:
                    same=True
                    indexColor.append(i)
                    break
            
            if not same:
                indexColor.append(len(uniqueColor))
                uniqueColor.append(u);
    else:
        for poly in object.data.polygons:
            same = False
            for i in range(0,len(uniqueColor)):
                if uniqueColor[i][2]==poly.material_index:
                    same=True
                    indexColor.append(i)
                    break
            
            if not same:
                for i in poly.vertices:
                    indexColor.append(len(uniqueColor))
                uniqueColor.append([0,0,poly.material_index]);
       
    colorC=len(uniqueColor)
    
    uniqueWeight=[]
    indexWeight=[]
    if useArmature:
        for ve in object.data.vertices:
            weight=[0,0,0,0,0,0]
            # Grab the most important weights
            for gr in ve.groups:
                lowW=gr.weight
                lowB=original.modifiers["Armature"].object.data.bones.find(object.vertex_groups[gr.group].name)
                for i in range(0,3):
                    if lowW>weight[3+i]:
                        temp=[weight[i],weight[3+i]]
                        weight[i]=lowB
                        weight[3+i]=lowW
                        lowB=temp[0]
                        lowW=temp[1]
            
            same=False
            for i in range(0,len(uniqueWeight)):
                same2=True
                for j in range(0,6):
                    if not uniqueWeight[i][j]==weight[j]:
                        same2=False
                        break
                
                if same2:
                    same=True
                    indexWeight.append(i)
                    break
            
            if not same:
                indexWeight.append(len(uniqueWeight))
                uniqueWeight.append([weight[0],weight[1],weight[2],weight[3],weight[4],weight[5]])
                #leng=weight[3]+weight[4]+weight[5]
                #file.write(struct.pack("=BBBfff",weight[0]+1,weight[1]+1,weight[2]+1,weight[3]/leng,weight[4]/leng,weight[5]/leng))

    weightC=len(uniqueWeight)
    
    # Write Mesh Information
    
    
    file.writeComment("MeshType")
    file.writeUChar(1 if useArmature else 0)
    file.writeComment("Vertex Count")
    file.writeUShort(verC)# Vertex Count (Maximum 65536)
    file.writeComment("Color Count")
    file.writeUShort(colorC)
    
    file.writeComment("Materials")
    file.writeUChar(len(object.data.materials))
    for mat in object.data.materials:
        file.writeString(mat.name)
        
    if useArmature:
        file.writeComment("Weight Count")
        file.writeUShort(weightC)
    file.writeComment("Triangle count")
    file.writeUShort(triC)# Triangle Count (Maximum 65536)
    #print("Points "+str(verC)+" Textures "+str(colorC)+" Triangles" +str(triC)+" Mats "+str(len(object.data.materials)))
    
    # Write Mesh Data
    file.writeComment("Vertex data")
    for ver in object.data.vertices:
        v = Vector((ver.co[0],ver.co[1],ver.co[2],0))
        #v = Vector((ver.co[0],ver.co[2],-ver.co[1],0))
        #if not useArmature:
        #    v = Vector((ver.co[0],ver.co[1],ver.co[2],0))
        file.writeFloat(v[0],v[1],v[2])# SWITCH Y Z
    
    file.writeComment("Color data")
    for c in uniqueColor:
        file.writeFloat(c[0],c[1],c[2])
       # file.writeFloat(c[0],c[1],c[2])
            
    if useArmature:
        file.writeComment("Weight data")
        for w in uniqueWeight:
            leng=w[3]+w[4]+w[5]
            file.writeUChar(w[0],w[1],w[2])
            file.writeFloat(w[3]/leng,w[4]/leng,w[5]/leng)
  
    # Write index information for Triangles
    file.writeComment("Triangle data")
    for poly in object.data.polygons:
        v = poly.vertices
        lo = poly.loop_indices
        if not useArmature:
            if len(v)==3:
                l=[indexColor[lo[0]],indexColor[lo[1]],indexColor[lo[2]]]
                #print(v[1],l[1],v[2],l[2],v[0],l[0])
                #file.writeUShort(v[1],l[1],v[2],l[2],v[0],l[0])
                file.writeUShort(v[0],l[0],v[1],l[1],v[2],l[2])
#            elif len(v)==4:
#                l=[indexColor[lo[0]],indexColor[lo[1]],indexColor[lo[2]],indexColor[lo[3]]]
#                file.write(struct.pack("=HHHHHH",v[1],l[1],v[2],l[2],v[0],l[0]))
#                file.write(struct.pack("=HHHHHH",v[2],l[2],v[3],l[3],v[0],l[0]))
            else:
                Logging('WARNING',"Triangulate failed in "+original.name)
                #print("Cannot have more than five vertices per quad!")
        else:
            if len(v)==3:
                l=[indexColor[lo[0]],indexColor[lo[1]],indexColor[lo[2]]]
                w=[indexWeight[v[0]],indexWeight[v[1]],indexWeight[v[2]]]
                file.writeUShort(v[1],l[1],w[1],v[2],l[2],w[2],v[0],l[0],w[0])
#            elif len(v)==4:
#                l=[indexColor[lo[0]],indexColor[lo[1]],indexColor[lo[2]],indexColor[lo[3]]]
#                w=[indexWeight[v[0]],indexWeight[v[1]],indexWeight[v[2]],indexWeight[v[3]]]
#                file.write(struct.pack("=HHHHHHHHH",v[1],l[1],w[1],v[2],l[2],w[2],v[0],l[0],w[0]))
#                file.write(struct.pack("=HHHHHHHHH",v[2],l[2],w[2],v[3],l[3],w[3],v[0],l[0],w[0]))
            else:
                Logging('WARNING',"Triangulate failed in "+original.name)
                #print("Cannot have more than five vertices per quad!")
    
    Logging('INFO',original.data.name)
    
    # Cleanup
    file.close()
    bpy.ops.object.delete()

def WriteAnimation(path, action, object):
  
    file = FileWriter(path+action.name+".animation")
    if file.error:
        return

    # Write Anim Information
    fstart = int(action.frame_range[0])
    fend = int(action.frame_range[1])

    file.writeComment("Start frame")
    file.writeUShort(fstart)
    file.writeComment("End frame")
    file.writeUShort(fend)
    file.writeComment("FPS")
    file.writeFloat(25) # Frame per second
    
    objects=0
    for group in action.groups:
        if group.name=="Object Transforms": # object is 
            objects=objects+1
            continue
        else: # object is always armature here
            id = object.data.bones.find(group.name) # animation for mesh is not possible
            # remove animtion in drivers for the mesh object. bro, why and what does this mean
            if not id==-1:
                objects+=1
           
    file.writeComment("Object count")
    file.writeUChar(objects)
    #print("Animation "+action.name)
    # Write Data
    file.writeComment("Animnation data")
    xyz="XYZ"
    for group in action.groups:
        if group.name=="Object Transforms":
            file.writeUShort(0)
        else:
            id = object.data.bones.find(group.name)
            #print(str(id)+ " "+group.name)
            if id==-1:
                # group name not found
                continue
            else:
                file.writeUShort(id)
           
        curveInt=0
        curves=[0,0,0,0,0,0,0,0,0,0,0,0,0]
        for curve in group.channels:
            if curve.is_empty or curve.mute or not curve.is_valid:# is this safe?
                continue
            
            con=0
            if curve.data_path[-8:]=="location":
                con=0
            elif curve.data_path[-5:]=="euler":
                con=3#print("Euler for rotation is not allowed please change it to Quaternions. Check python script for more information.")
            elif curve.data_path[-5:]=="scale":
                con=6
            elif curve.data_path[-10:]=="quaternion":
                con=9
            
            # Code used to switch x and w for quaternion
            if con>8:
                if curve.array_index==0:
                    con=con+3
                else:
                    con=con+curve.array_index-1
            else:
                con=con+curve.array_index
            
            curves[con]=1
                    
        for i in range(0,13):
            curveInt += curves[i]*pow(2,i)
            
        file.writeUShort(curveInt)
        
        for i in range(0,13):
            
            if curves[i]==0:
                continue
            
            for curve in group.channels:
                
                con=0
                if curve.data_path[-8:]=="location":
                    con=0
                elif curve.data_path[-5:]=="euler":
                    con=3
                elif curve.data_path[-5:]=="scale":
                    con=6
                elif curve.data_path[-10:]=="quaternion":
                    con=9
                
                # Code used to switch x and w for quaternion
                if con>8:
                    if curve.array_index==0:
                        con=con+3
                    else:
                        con=con+curve.array_index-1
                else:
                    con=con+curve.array_index
                        
                #print(str(con)+" "+curve.data_path+" "+str(curve.array_index))
                # switch x and z
                if not con==i:
                        continue
                
                file.writeUShort(len(curve.keyframe_points))
                
                for key in curve.keyframe_points:
                    pol=key.interpolation
                    if pol=="CONSTANT":
                        pol=0
                    elif pol=="LINEAR":
                        pol=1
                    elif pol=="BEZIER":
                        pol=2
                    else:
                        pol=0
                    
                    file.writeUChar(pol)
                    file.writeUShort(int(key.co[0]))
                    file.writeFloat(key.co[1])
    
    Logging('INFO',action.name)
    
    # Cleanup
    file.close()

def WriteMaterial(path,material):
    
    file = FileWriter(path+material.name+".material")
    if file.error:
        return

    textureName=""
    color = material.diffuse_color
    if material.use_nodes:
        surfaceNode = material.node_tree.nodes["Material Output"].inputs["Surface"].links[0].from_node
        base = surfaceNode.inputs["Base Color"]
        color = [1,1,1]
        if len(base.links)>0:
            if base.links[0].from_node.name=="Image Texture":
                textureName = base.links[0].from_node.image.name
                if len(textureName)>4:
                    if textureName[-4:]==".png":
                        textureName = textureName[:-4]
                
                # copy file to the material´s
                shutil.copyfile(bpy.path.abspath("//")+base.links[0].from_node.image.filepath,path+textureName+".png")
            else:
                Logging(log,'WARNING',"Only Image Texture node allowed in Material "+material.name)
        else:
            color = base.default_value
    
    file.writeString(textureName)
    file.writeComment("Color")
    file.writeFloat(color[0],color[1],color[2])
    file.writeComment("Specular")
    file.writeFloat(material.specular_color[0],material.specular_color[1],material.specular_color[2])
    file.writeComment("Intensity")
    file.writeFloat(material.specular_intensity)
    
    Logging('INFO',material.name)
    
    # Cleanup
    file.close()

def WriteArmature(path, original):

    file = FileWriter(path+original.data.name+".armature")
    if file.error:
        return
    
    bpy.ops.object.select_all(action="DESELECT")
    # Make a copy of the object and apply modifiers
    object=original.copy()
    object.data=original.data.copy()
    bpy.context.scene.collection.objects.link(object)
    object.select_set(True)
    bpy.context.view_layer.objects.active=object
                       
    for mod in object.modifiers:
        bpy.ops.object.modifier_apply(modifier=mod.name)
    
    # Loading data
    file.writeComment("Bone count")
    file.writeUChar(len(object.data.bones))
    file.writeComment("Bone data")
    for bone in object.data.bones:
        parentIndex = object.data.bones.find(bone.parent.name) if bone.parent else 0 # the first bone will not use the parent index
        #print(index)
        #if bone.parent:
        #    print(str(object.data.bones.find(bone.parent.name)) + " " +bone.parent.name)
        # axisConvert to switch Y-axis and Z-axis which is done in the model.
        #  applying axisConvert to root bone will apply it to all other bones as well
        local_matrix = bone.matrix_local
        if bone.parent:
            local_matrix = (bone.parent.matrix_local).inverted() @ bone.matrix_local
       
        local_matrix = local_matrix.transposed()
        inv_model_matrix = bone.matrix_local.inverted().transposed()
        
        file.writeUShort(parentIndex)
        file.writeMatrix(local_matrix)
        file.writeMatrix(inv_model_matrix)
      
    Logging('INFO',original.data.name)
    # Cleanup
    file.close()
    bpy.ops.object.delete()

def WriteCollider(path,original):
    
    # Create File
    try:
        file = open(path+original.data.name+".collider","wb")
    except FileNotFoundError:
        Logging('ERROR','Directory not found '+path)
        return
    else:
        pass
    
    bpy.ops.object.select_all(action="DESELECT")
    # Make a copy of the object and apply modifiers
    object=original.copy()
    object.data=original.data.copy()
    bpy.context.scene.collection.objects.link(object)
    object.select_set(True)
    bpy.context.view_layer.objects.active=object
    
    # Uncomment when triangles are supported
    #if object.modifiers.find("Triangulate")==-1:
    #    object.modifiers.new("Triangulate",'TRIANGULATE')
    
    for mod in object.modifiers:
        bpy.ops.object.modifier_apply(modifier=mod.name)
    
    # Determine amount of data
    furthest=0
    for v in object.data.vertices:
        l=math.sqrt(v.co.x*v.co.x+v.co.y*v.co.y+v.co.z*v.co.z)
        if l>furthest:
            furthest=l
    tC=0
    qC=0
    for poly in object.data.polygons:
        if len(poly.vertices)>3:
            qC=qC+1
        else:
            tC=tC+1
    
    # Write Coll Information - Add in an unsigned int for tC
    file.write(struct.pack("=HHf",len(object.data.vertices),qC,furthest))
   
    # Write Points
    for ver in object.data.vertices:
        v = axisConvert@ver.co
        file.write(struct.pack("=fff",v[0],v[1],v[2]))
    
    # Write Planes
    for poly in object.data.polygons:
        v = poly.vertices
        
        if len(v)==4:
            file.write(struct.pack("=HHHH",v[0],v[1],v[2],v[3]))
        else:
            pass # Bring this back when triangles are supported
            #file.write(struct.pack("=HHH",v[0],v[1],v[2]))
    
    if tC>0:
        Logging('WARNING',"Ignoring "+tC+" triangles from "+original.name)
   
    Logging('INFO',original.data.name)
    # Cleanup
    file.close()
    bpy.ops.object.delete()

def WriteModel(path,name,meshes,armatures,colliders,animations):
    
    file = FileWriter(path+name+".model")
    if file.error:
        return
    
    general = []
    for o in meshes:
        general.append(o)
    for o in armatures:
        general.append(o)
    for o in colliders:
        general.append(o)
    
    sort = sorted(general,key=lambda x: general.index(x.parent) if x.parent else -1)
    
    file.writeComment("Instance count")
    file.writeUChar(len(sort))
    for o in sort:
        parentIndex = sort.index(o.parent) if o.parent else -1
        
        local_matrix = (axisConvert@o.matrix_local)
        #local_matrix = (o.matrix_local)
        if o.parent:
            local_matrix = o.matrix_local
        local_matrix = (local_matrix).transposed()
        
        #inv_model_matrix = o.matrix_local.inverted()
        
        type = 0
        if o in meshes:
            type=0
        if o in armatures:
            type=1
        if o in colliders:
            type=2
        file.writeString(o.name)
        file.writeUChar(type)
        if type==0:
            file.writeString(o.data.name+o["MESHTYPE"])
        else:
            file.writeString(o.data.name)
        file.writeShort(parentIndex)
        file.writeMatrix(local_matrix)
        #file.writeMatrix(inv_model_matrix.transposed())
    
    file.writeComment("Animation count")
    file.writeUShort(len(animations))
    for anim in animations:
        file.writeString(anim.name)
    
    Logging('INFO',name)
    
    # Cleanup
    file.close()

def LoadFiles(assetPath):
    global log
    #log=[,]
    #Logging('INFO','Loaded ')
    log=['INFO','Loaded ']
    
    #modelName=""
    #allObjects=[]
    if not bpy.context.mode=='OBJECT':
        Logging('ERROR',"Must be in object mode!")
    else:
        collection=bpy.context.collection
          
        #print(assetPath)
        objectPath=assetPath
            
        check_objects=[]
        
        individual=False
        if len(bpy.context.selected_objects)>0:
            check_objects=bpy.context.selected_objects
            individual=True
        elif len(bpy.context.collection.children)==0:# no sub collections
            check_objects=collection.objects
            objectPath+=collection.name+"\\"
            os.makedirs(os.path.dirname(objectPath),exist_ok=True)
            #print(len(bpy.context.collection.objects))
        
        #print(individual,len(check_objects))
        
        TYPE_NORMAL="-N"
        TYPE_BONED="-B"
        
        # these are the instances of the objects
        loadedMeshes = []
        loadedArmatures = []
        loadedColliders = []
        loadedAnimations = []
            
        # model i the term for the collection
        # mesh is a unique mesh asset
        # instances is a transform which refers to a mesh
        # material is the color or texturing of a mesh
        
        for child in check_objects:
            #print(objectPath)
            if not child.hide_get():
                # Write animation
                if not child.animation_data==None:
                    for track in child.animation_data.nla_tracks:
                        for strip in track.strips:
                            action=strip.action
                    
                            isLoaded = False
                            for act in loadedAnimations:
                                if act.name == action.name:
                                    isLoaded=True
                                    break
                            
                            if not isLoaded:
                                WriteAnimation(objectPath,action,child)
                                print(action.name)
                                loadedAnimations.append(action)
                            
                
                # Write mesh
                if child.type=='MESH':
                    if child.name[len(child.name)-1]=="C":
                        isLoaded=False
                        for l in loadedColliders:
                            if l.data.name==child.data.name:
                                isLoaded=True
                        
                        if not isLoaded:
                            WriteCollider(objectPath,child)
                        
                        loadedColliders.append(child)
                        
                    
                    # Write collider      
                    else:
                        meshType = TYPE_NORMAL
                        #print(child)
                        #print(child.modifiers)
                        for mod in child.modifiers:
                            #print(" ",mod.type)
                            if mod.type=='ARMATURE':
                                meshType=TYPE_BONED
                                
                        isLoaded=False
                        for l in loadedMeshes:
                            if l.data.name==child.data.name:
                                if l["MESHTYPE"]==meshType:
                                    isLoaded=True
                                    break
                                
                        #print("AAA ",child.data.name,meshType)
                        child["MESHTYPE"]=meshType
                        if not isLoaded:
                            WriteMesh(objectPath,child)
                            for mat in child.data.materials:
                                WriteMaterial(objectPath,mat)
                                
                        loadedMeshes.append(child)
                            
                # Write Armature    
                elif child.type=='ARMATURE':
                    isLoaded=False
                    for l in loadedArmatures:
                        if l.data.name==child.data.name:
                            isLoaded=True
                    
                    if not isLoaded:
                        WriteArmature(objectPath,child)
                    
                    loadedArmatures.append(child)
                    
        if not individual:
            WriteModel(objectPath,collection.name,loadedMeshes,loadedArmatures,loadedColliders,loadedAnimations)   

    # select previously selected objects

    if log[0]=='INFO':
        log[1]=log[1][:-2]
    
    return log

# The code below is for prefererences
class MyProperty(bpy.types.PropertyGroup):
    myProjectPath : bpy.props.StringProperty(name="Project Path")
    mySubPath : bpy.props.StringProperty(name="Sub Path")
    
class MExportPanel(bpy.types.Panel):
    bl_label = "Model Exporter"
    bl_idname = "mexport_panel"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = "Model Exporter"
    
    def draw(self,context):
        layout = self.layout
        scene = context.scene
        mexport = scene.mexport
        
        layout.prop(mexport,"myProjectPath")
        layout.prop(mexport,"mySubPath")
        layout.operator("object.mexport_models");

# The code below is for exporting models
class MExportOperator(bpy.types.Operator):
    bl_label = "Export Models"
    bl_idname = "object.mexport_models"
    
    def execute(self, context):
        scene = context.scene
        me = scene.mexport
    
        # Default path once again
        if me.myProjectPath=="":
            me.myProjectPath="D:\\Development\\VisualStudio\\ProjectUnknown\\ProjectUnknown\\assets\\"
        
        # No need to worry about ending the with slashes
        projectPath = me.myProjectPath if me.myProjectPath[-1]=='\\' else me.myProjectPath+'\\'
        if len(me.mySubPath)>0:
            projectPath = projectPath + (me.mySubPath if me.mySubPath[-1]=='\\' else me.mySubPath+'\\')
        
        LoadFiles(projectPath)
        
        self.report({log[0]}, log[1])
        return {"FINISHED"}
    
classes = [MyProperty,MExportPanel,MExportOperator]

def register():
    for cls in classes:
        bpy.utils.register_class(cls)
        
        bpy.types.Scene.mexport = bpy.props.PointerProperty(type=MyProperty)
        
    # Default path
    me = bpy.context.scene.mexport
    if me.myProjectPath=="":
        # Example path
        me.myProjectPath="D:\\Development\\VisualStudio\\ProjectUnknown\\ProjectUnknown\\assets\\"
 
def unregister():
    for cls in classes:
        bpy.utils.unregister_class(cls)
        
        del bpy.types.Scene.mexport
    
if __name__ == "__main__":
    register()