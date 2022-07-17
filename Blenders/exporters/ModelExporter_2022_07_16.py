# Script by Emarioo / Dataolsson, Updated 2022-07-11
# Watch Dataolsson on youtube for more information
# If any error occurs please contact Dataolsson on youtube so I can update this script

bl_info = {
"name": "Model Exporter",
"category": "Object",
"author": "Dataolsson",
"description": "Menu and panel for exporting blender models, animations and armatures."
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

logState="INFO"
infoLog=[]
warningLog=[]
errorLog=[]


# Logger

def ResetLog():
    global logState
    global infoLog
    global warningLog
    global errorLog
    
    logState="INFO"
    infoLog=[]
    warningLog=[]
    errorLog=[]

def Logging(state,msg):
    global logState
    global infoLog
    global warningLog
    global errorLog
    
    if state=="INFO":
        infoLog.append(msg)
        
    elif state=="WARNING":
        if not logState=="ERROR":
            logState="WARNING"
        warningLog.append(msg)
        
    elif state=="ERROR":
        logState="ERROR"
        errorLog.append(msg)
        
def FinalLog():
    if logState=="INFO":
        msg="Loaded "
        for i in range(0,len(infoLog)):
            if not i==0:
                msg+=", "
            msg+=infoLog[i]
        return msg
    elif logState=="WARNING":
        msg=""
        for i in range(0,len(warningLog)):
            if not i==0:
                msg+=", "
            msg+=warningLog[i]
        return msg
    elif logState=="ERROR":
        msg=""
        for i in range(0,len(errorLog)):
            if not i==0:
                msg+=", "
            msg+=errorLog[i]
        return msg

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
                if value[i]==0:
                    self.file.write('0')
                elif value[i]==1:
                    self.file.write('1')
                else:
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
    original["dataName"]=original.data.name+original["MESHTYPE"]
    file = FileWriter(path+original["dataName"]+".mesh")
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
    # animation does not need dataName
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
    
    temp = ["X","Y","Z","rX","rY","rZ","sX","sY","sZ","qX","qY","qZ","qW"]
   
    # Write Data
    file.writeComment("Animnation data")
    xyz="XYZ"
    for group in action.groups:
        file.writeComment("Object")
        if group.name=="Object Transforms":
            file.writeUShort(0)
            Logging('WARNING',"Object transforms aren't supported")
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
                con=3
                Logging("WARNING","Please use quaternions instead of euler")
                #print("Euler for rotation is not allowed please change it to Quaternions. Check python script for more information.")
            elif curve.data_path[-5:]=="scale":
                con=6
            elif curve.data_path[-10:]=="quaternion":
                con=9
            
            # Code used to switch x and w for quaternion  (glm and blender is different)
            if con>8:
                if curve.array_index==0:
                    con=con+3
                else:
                    con=con+curve.array_index-1
            else:
                con=con+curve.array_index
            
            curves[con]=1
            
       # channelType=""        
        for i in range(0,13):
            #if curves[i]==1:
            #    channelType+=" "+temp[i]
            curveInt += curves[i]*pow(2,i)
        
        file.writeComment("Channels")
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
                
                # Code used to switch x and w for quaternion (glm and blender is different)
                if con>8:
                    if curve.array_index==0:
                        con=con+3
                    else:
                        con=con+curve.array_index-1
                else:
                    con=con+curve.array_index
                
                if not con==i:
                    continue
                
                
                file.writeComment("Frames for "+temp[i])
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
                    file.writeComment("Frame");
                    file.writeUChar(pol)
                    file.writeUShort(int(key.co[0]))
                    file.writeFloat(key.co[1])
    
    Logging('INFO',action.name)
    
    # Cleanup
    file.close()

def WriteMaterial(path,material):
    # material is special and does not have dataName
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
    original["dataName"]=original.data.name
    file = FileWriter(path+original["dataName"]+".armature")
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

def GetColliderType(object):
    if object.data.name[:4]=="CUBE":
        return "CUBE"
    elif object.data.name[:6]=="SPHERE":
        return "SPHERE"
    elif object.data.name[:3]=="MAP":
        return "MAP"
    return False

def rounding(x):
    return round(x*1000)/1000

# cube collider does not use original.data
def WriteCollider(path,original):
    colliderType = GetColliderType(original)
    original["dataName"]=original.name+"_"+colliderType
    file = FileWriter(path+original["dataName"]+".collider")
    if file.error:
        return
    
    # object does not have any modifiers which needs to be applied so we don't need to make a copy
    object=original
    #bpy.ops.object.select_all(action="DESELECT")
    # Make a copy of the object and apply modifiers
    #object=original.copy()
    #object.data=original.data.copy()
    #bpy.context.scene.collection.objects.link(object)
    #object.select_set(True)
    #bpy.context.view_layer.objects.active=object

    # CUBE 0
    # SPEHRE 1
    # MAP 2
    
    file.writeComment("Type")
    if colliderType=="CUBE":
        file.writeUChar(0)
        file.writeComment("Size")
        # dimensions is used here to get the size of the cube. Scale would give the wrong value.
        file.writeFloat(object.dimensions[0]/2,object.dimensions[1]/2,object.dimensions[2]/2)
        
        Logging('INFO',original.name)
    elif colliderType=="SPHERE":
        file.writeUChar(0)
        file.writeComment("radius")
        radius = sqrt(pow(object.dimensions[0]/object.scale[0],2)+pow(object.dimensions[1]/object.scale[1],2)+pow(object.dimensions[2]/object.scale[2],2))/2
        file.writeFloat(radius);
        file.writeComment("Position")
        offset = GetMeshOffset(object)
        file.writeFloat(offset.x,offset.y,offset.z)
        
        Logging('INFO',original.name)
    elif colliderType=="MAP":
        # HEIGHT MAP ASSUMES SOME STUFF
        # Y is up, this means the map needs to be on it's side
        file.writeUChar(2)
        minHeight=9999999
        maxHeight=-9999999
        for v in object.data.vertices:
            if v.co.y>maxHeight:
                maxHeight=v.co.y
            if v.co.y<minHeight:
                minHeight=v.co.y
        
        width = math.sqrt(len(object.data.vertices))
        
        if width==int(width):
            width=int(width)
            centerPoint = Vector()
            for v in object.data.vertices:
                centerPoint += v.co
                
            centerPoint/=len(object.data.vertices) # this line may not be needed since i don't need the middle point, i just need to know if the center is at (0,0,0)
            centerPoint.x=rounding(centerPoint.x)
            centerPoint.y=rounding(centerPoint.y)
            centerPoint.z=rounding(centerPoint.z)
           
            if centerPoint.x==0 and centerPoint.z == 0:
                print("CenterPoint:",centerPoint)
                Logging('WARNING',"Height map is not centered or not following grid system!")
                
            file.writeComment("Width")
            file.writeUShort(width)
            file.writeComment("Height")
            file.writeUShort(width)
            
            # this will make sure that the median of min and max is zero.
            if abs(minHeight)<abs(maxHeight):
                minHeight=-maxHeight
            else:
                maxHeight=-minHeight
            
            
            file.writeComment("Min Height")
            file.writeFloat(minHeight)
            file.writeComment("Max Height")
            file.writeFloat(maxHeight)
            # this changes depending on the distance between points, NOT the transform(matrix_local)
            file.writeComment("ScaleXZ")
            scaleX = original.dimensions.x/(width-1)
            scaleY = original.scale.y
            scaleZ = original.dimensions.z/(width-1)
            print("Scale: ",scaleX,scaleY,scaleZ)
            file.writeFloat(scaleX,scaleY,scaleZ)
            file.writeComment("Height Data")
        
             # The vertices are not stored in order, which means i need to do that.
            data = []
            for i in range(0,len(object.data.vertices)):
                data.append(0) # maybe not the best way to fill an array
            
            baseX = original.dimensions.x/2
            baseZ = original.dimensions.z/2
            print("BaseXZ: ",baseX,baseZ," Width: ",width)
            for v in object.data.vertices:
                xi = round((baseX+v.co.x)/scaleX)
                zi = round((baseZ+v.co.z)/scaleZ)
                
                # index should hopefully be less than len(data), if it's not, it's a bug.
                index = zi*(width)+xi
                #print("XY: ",v.co.x,v.co.z," Index: ",xi,zi)
                    
                data[index]=v.co.y
            
            for val in data:
                file.writeFloat(val)
            
            Logging('INFO',original.name)
        else:
            Logging('ERROR',"Height map is not square!")
        
        
    elif colliderType=="MESH":
        file.writeUChar(2)
        
        furthest=0
        for v in object.data.vertices:
            dist=v.co.x*v.co.x+v.co.y*v.co.y+v.co.z*v.co.z
            if dist>furthest:
                furthest=dist
        furthest=math.sqrt(furthest)
        
        triCount=0
        for poly in object.data.polygons:
            if len(poly.vertices)==3:
                triCount+=1
        
        file.writeComment("Vertex count")
        file.writeUShort(len(object.data.vertices))
        file.writeComment("Triangle count")
        file.writeUShort(triCount)
        file.writeComment("Furthest point")
        file.writeFloat(furthest)
        
        # Write Points
        for ver in object.data.vertices:
            v = ver.co
            file.writeFloat(v[0],v[1],v[2])
            #file.write(struct.pack("=fff",v[0],v[1],v[2]))
        
        # Write Planes
        for poly in object.data.polygons:
            v = poly.vertices
            if len(v)==3:
                file.writeUShort(v[0],v[1],v[2])
        
        Logging('INFO',original.name)        
    else:
        Logging('WARNING',"Collider "+colliderType+" is not supported!")
   
    # Cleanup
    file.close()
    #bpy.ops.object.delete()

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
        
        localMatrix=o.matrix_local
        
        #if parentIndex==-1:
       #     local_matrix = local_matrix.Translation(local_matrix[3])
        
        type = 0
        if o in meshes:
            type=0
            meshes.remove(o)
        if o in armatures:
            type=1
            armatures.remove(o)
        if o in colliders:
            type=2
            colliders.remove(o)
        
        # ignore scale
        if type==2:
            localMatrix = Matrix.LocRotScale(localMatrix.to_translation(),localMatrix.to_quaternion(),None)
        
        file.writeString(o.name)
        file.writeComment("Instance Type")
        file.writeUChar(type)
        
        file.writeString(o["dataName"])
            
        file.writeShort(parentIndex)
        file.writeMatrix(localMatrix.transposed())
    
    file.writeComment("Animation count")
    file.writeUShort(len(animations))
    for anim in animations:
        file.writeString(anim.name)
    
    Logging('INFO',name)
    
    # Cleanup
    file.close()

def LoadFiles(assetPath):
    if not bpy.context.mode=='OBJECT':
        Logging('ERROR',"Must be in object mode!")
    elif len(bpy.context.selected_objects)==0:
        Logging('ERROR',"Select an object!")
    else:
        selectedObjects = bpy.context.selected_objects
        activeObject = bpy.context.object
        
        export_objects=bpy.context.selected_objects
        for rootObject in export_objects: 
            if rootObject.parent==None:
          
                objectPath=assetPath+rootObject.name+"\\"
                    
                os.makedirs(os.path.dirname(objectPath),exist_ok=True)
                
                TYPE_NORMAL="-N"
                TYPE_BONED="-B"
                
                # these are the instances of the assets/objects
                loadedMeshes = []
                loadedArmatures = []
                loadedColliders = []
                loadedAnimations = []
                            
                # model is the term for the root object
                # mesh is a unique mesh asset
                # instances are a transform which refers to a mesh
                # material is the color or texturing of a mesh
                objectsLeft=[rootObject]
                while len(objectsLeft)>0:
                    child=objectsLeft[0]
                    objectsLeft.remove(child)
                    
                    # This bit will ignore hidden objects
                    if child.hide_get():
                        continue
                    
                    for o in child.children: 
                        objectsLeft.append(o)
                    
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
                                    loadedAnimations.append(action)
                                
                    
                    # Write mesh and/or collider
                    if child.type=='MESH':
                        if child.name[:2]=="C_":
                            # THIS NEEDS TO CHANGE WHEN DOING MESHES
                            WriteCollider(objectPath,child)
                            loadedColliders.append(child)
                            continue
                        
                        meshType = TYPE_NORMAL
                        
                        for mod in child.modifiers:
                            if mod.type=='ARMATURE':
                                meshType=TYPE_BONED
                                    
                        isLoaded=False
                        for l in loadedMeshes:
                            if l.data.name==child.data.name:
                                if l["MESHTYPE"]==meshType:
                                    isLoaded=True
                                    break
                                    
                        
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
                            
                WriteModel(objectPath,rootObject.name,loadedMeshes,loadedArmatures,loadedColliders,loadedAnimations)   

        # select previously selected objects
        for ob in selectedObjects:
            bpy.ops.object.select_pattern(pattern=ob.name)
        
        if not activeObject==None:
            bpy.context.view_layer.objects.active=activeObject

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
            Logging("ERROR","Project path is empty!")
            #me.myProjectPath="D:\\Development\\VisualStudio\\ProjectUnknown\\ProjectUnknown\\assets\\"
        
        # No need to worry about ending the with slashes
        projectPath = me.myProjectPath if me.myProjectPath[-1]=='\\' else me.myProjectPath+'\\'
        if len(me.mySubPath)>0:
            projectPath = projectPath + (me.mySubPath if me.mySubPath[-1]=='\\' else me.mySubPath+'\\')
        
        ResetLog()
        
        LoadFiles(projectPath)
        
        global logState
        msg = FinalLog()
        
        self.report({logState}, msg)
        return {"FINISHED"}
    
classes = [MyProperty,MExportPanel,MExportOperator]

def register():
    for cls in classes:
        bpy.utils.register_class(cls)
        
        bpy.types.Scene.mexport = bpy.props.PointerProperty(type=MyProperty)
        
    # Default path
    me = bpy.context.scene.mexport
   # if me.myProjectPath=="":
    #    # Example path
     #   me.myProjectPath="D:\\Development\\VisualStudio\\ProjectUnknown\\ProjectUnknown\\assets\\"
        
 
def unregister():
    for cls in classes:
        bpy.utils.unregister_class(cls)
        
        del bpy.types.Scene.mexport
    
if __name__ == "__main__":
    register()