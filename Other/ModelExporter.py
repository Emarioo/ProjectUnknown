# Script by Emarioo / Dataolsson, Updated 2021-01-20
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

# Used to switch the axis (Y-up instead of Z-up)
axisConvert = Matrix((
(1.0, 0.0, 0.0, 0.0),
(0.0, 0.0, 1.0, 0.0),
(0.0,-1.0, 0.0, 0.0),
(0.0, 0.0, 0.0, 1.0)))
        
def WriteMesh(path,original):
    out=['INFO',original.data.name]
    
    # Create File
    try:
        file = open(path+original.data.name+".mesh","wb")# CHANGED
    except FileNotFoundError:
        return ['ERROR','Directory not found '+path]
    else:
        pass
        
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
                            
    for mod in object.modifiers:
        if not mod.type=='ARMATURE':
            bpy.ops.object.modifier_apply(modifier=mod.name)
    
    triC=0
    for poly in object.data.polygons:
        if len(poly.vertices)==4:
            triC=triC+2
        else:
            triC=triC+1
    useArmature=False
    for mod in original.modifiers:
        if mod.type=='ARMATURE':
            useArmature=True
            break
    
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
    file.write(struct.pack("=B",1 if useArmature else 0))
    file.write(struct.pack("=H",verC))# Vertex Count (Maximum 65536)
    file.write(struct.pack("=H",colorC))
    file.write(struct.pack("=B",len(object.data.materials)))
    for mat in object.data.materials:
        file.write(struct.pack("=B",len(mat.name)))
        file.write(bytearray(mat.name,"UTF-8"))
        
    if useArmature:
        file.write(struct.pack("=H",weightC))
    file.write(struct.pack("=H",triC))# Triangle Count (Maximum 65536)
    
    # Write Mesh Data
    for ver in object.data.vertices:
        v = Vector((ver.co[0],ver.co[1],ver.co[2],0))
        #if not useArmature:
        #    v=axisConvert@v
        file.write(struct.pack("=fff",v[0],v[1],v[2]))# SWITCH Y Z
    
    for c in uniqueColor:
        file.write(struct.pack("=fff",c[0],c[1],c[2]))
            
    if useArmature:
        for w in uniqueWeight:
            leng=w[3]+w[4]+w[5]
            file.write(struct.pack("=BBBfff",w[0],w[1],w[2],w[3]/leng,w[4]/leng,w[5]/leng))
  
    # Write index information for Triangles
    for poly in object.data.polygons:
        v = poly.vertices
        lo = poly.loop_indices
        if not useArmature:
            if len(v)==3:
                l=[indexColor[lo[0]],indexColor[lo[1]],indexColor[lo[2]]]
                file.write(struct.pack("=HHHHHH",v[1],l[1],v[2],l[2],v[0],l[0]))
#            elif len(v)==4:
#                l=[indexColor[lo[0]],indexColor[lo[1]],indexColor[lo[2]],indexColor[lo[3]]]
#                file.write(struct.pack("=HHHHHH",v[1],l[1],v[2],l[2],v[0],l[0]))
#                file.write(struct.pack("=HHHHHH",v[2],l[2],v[3],l[3],v[0],l[0]))
            else:
                out=['WARNING',"Triangulate failed in "+original.name]
                #print("Cannot have more than five vertices per quad!")
        else:
            if len(v)==3:
                l=[indexColor[lo[0]],indexColor[lo[1]],indexColor[lo[2]]]
                w=[indexWeight[v[0]],indexWeight[v[1]],indexWeight[v[2]]]
                file.write(struct.pack("=HHHHHHHHH",v[1],l[1],w[1],v[2],l[2],w[2],v[0],l[0],w[0]))
#            elif len(v)==4:
#                l=[indexColor[lo[0]],indexColor[lo[1]],indexColor[lo[2]],indexColor[lo[3]]]
#                w=[indexWeight[v[0]],indexWeight[v[1]],indexWeight[v[2]],indexWeight[v[3]]]
#                file.write(struct.pack("=HHHHHHHHH",v[1],l[1],w[1],v[2],l[2],w[2],v[0],l[0],w[0]))
#                file.write(struct.pack("=HHHHHHHHH",v[2],l[2],w[2],v[3],l[3],w[3],v[0],l[0],w[0]))
            else:
                out=['WARNING',"Triangulate failed in "+original.name]
                #print("Cannot have more than five vertices per quad!")
    
    # Cleanup
    file.close()
    bpy.ops.object.delete()
    return out

def WriteAnimation(path, action, object):
    out = ['INFO', action.name]
    
    # Create file
    try:
        file = open(path+action.name+".animation","wb")
    except FileNotFoundError:
        return ['ERROR','Directory not found '+path]
    else:
        pass
    
    # Write Anim Information
    fstart = int(action.frame_range[0])
    fend = int(action.frame_range[1])

    file.write(struct.pack("=H",fstart))
    file.write(struct.pack("=H",fend))
    file.write(struct.pack("=f",25)) # Frame per second
    
    objects=0
    for group in action.groups:
        if group.name=="Object Transforms":
            objects=objects+1
            continue
        else:
            id = object.data.bones.find(group.name)
            if not id==-1:
                objects=objects+1
           
    
    file.write(struct.pack("=B",objects))
    #print("Animation "+action.name)
    # Write Data
    xyz="XYZ"
    for group in action.groups:
        if group.name=="Object Transforms":
            file.write(struct.pack("=I",0))
        else:
            id = object.data.bones.find(group.name)
            #print(str(id)+ " "+group.name)
            if id==-1:
                # group name not found
                continue
            else:
                file.write(struct.pack("=H",id))
           
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
            curveInt += curves[i]*pow(2,i);
            
        file.write(struct.pack("=H",curveInt));
        
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
                
                file.write(struct.pack("=H",len(curve.keyframe_points)))
                
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
                    
                    file.write(struct.pack("=BHf",pol,int(key.co[0]),key.co[1])) 
            
    # Cleanup
    file.close()
    return out

def WriteMaterial(path,material):
    out = ['INFO',material.name]
    
    # Creating file
    try:
        file = open(path+material.name+".material","wb")
    except FileNotFoundError:
        return ['ERROR','Directory not found '+path]
    else:
        pass
    
    textureName=""
    color = material.diffuse_color
    if material.use_nodes:
        surfaceNode = material.node_tree.nodes["Material Output"].inputs["Surface"].links[0].from_node
        base = surfaceNode.inputs["Base Color"]
        color = [1,1,1]
        if len(base.links)>0:
            if base.links[0].from_node.name=="Image Texture":
                tempName = base.links[0].from_node.image.name
                textureName = tempName[0:len(tempName)-4]
            else:
                out = ['WARNING',"Only Image Texture node allowed in Material "+material.name]
        else:
            color = base.default_value
    
    
    file.write(struct.pack("=B",len(textureName)))
    if len(textureName)>0:
        file.write(bytearray(textureName,"UTF-8"))
    
    file.write(struct.pack("=fff",color[0],color[1],color[2]))
    file.write(struct.pack("=fff",material.specular_color[0],material.specular_color[1],material.specular_color[2]))
    file.write(struct.pack("=f",material.specular_intensity))
    
    # Cleanup
    file.close()
    return out

def WriteArmature(path, original):
    out = ['INFO',original.data.name]
    
    # Creating file
    try:
        file = open(path+original.data.name+".armature","wb")
    except FileNotFoundError:
        return ['ERROR','Directory not found '+path]
    else:
        pass
    
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
    boneCount = len(object.data.bones)
    file.write(struct.pack("=B",boneCount))
    
    for bone in object.data.bones:
        index = object.data.bones.find(bone.parent.name) if bone.parent else 0 # the first bone will not use the parent index
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
        
        file.write(struct.pack("=H",index))
        file.write(struct.pack("=16f",*local_matrix[0],*local_matrix[1],*local_matrix[2],*local_matrix[3]))
        file.write(struct.pack("=16f",*inv_model_matrix[0],*inv_model_matrix[1],*inv_model_matrix[2],*inv_model_matrix[3]))
      
    # Cleanup
    file.close()
    bpy.ops.object.delete()
    return out

def WriteCollider(path,original):
    out=['INFO',original.data.name]
    
    # Create File
    try:
        file = open(path+original.data.name+".collider","wb")
    except FileNotFoundError:
        return ['ERROR','Directory not found '+path]
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
        out=['WARNING',"Ignoring "+tC+" triangles from "+original.name]
   
    # Cleanup
    file.close()
    bpy.ops.object.delete()
    return out

def WriteModel(path,name,objects,armature,animations,collider):
    out=['INFO',name]
    
    # Create File
    try:
        file = open(path+name+".model","wb")
    except FileNotFoundError:
        return ['ERROR','Directory not found '+path]
    else:
        pass
    
    file.write(struct.pack("=B",len(armature)))
    file.write(bytearray(armature,"UTF-8"))
    
    file.write(struct.pack("=B",len(animations)))
    for a in animations:
        file.write(struct.pack("=B",len(a)))
        file.write(bytearray(a,"UTF-8"))
    
    file.write(struct.pack("=B",len(objects)))
    for o in objects:
        file.write(struct.pack("=B",len(o.data.name)))
        file.write(bytearray(o.data.name,"UTF-8"))
 
        local_matrix = (axisConvert@o.matrix_local).transposed()
       
        file.write(struct.pack("=16f",*local_matrix[0],*local_matrix[1],*local_matrix[2],*local_matrix[3]))
    
    file.write(struct.pack("=B",len(collider)))
    file.write(bytearray(collider,"UTF-8"))
    
    # Cleanup
    file.close()
    return out

# make a better logging system
def LogPriority(IN,msg):
    if IN[0]=='INFO':
        if msg[0]=='INFO':
            IN[1]=IN[1]+msg[1]+", "
        else:
            IN=msg
    elif IN[0]=='WARNING':
        if msg[0]=='WARNING':
            IN[1]=IN[1]+msg[1]+", "
        elif msg[0]=='ERROR':
            IN=msg
    elif IN[0]=='ERROR':
        if msg[0]=='ERROR':
            IN[1]=IN[1]+msg[1]+", "
    
    return IN

def LoadFiles(modelPath,animationPath,materialPath,meshPath,colliderPath,armaturePath):
    out=['INFO','Loaded ']
    
    modelName=""
    allObjects=[]
    if bpy.context.mode=='OBJECT':
        if len(bpy.context.collection.children)==0:
            collection=bpy.context.collection
            modelName=collection.name
            
            for child in collection.objects:
                if not child.hide_get():
                    allObjects.append(child)
                    
        else:
            if not bpy.context.object==None:
                allObjects.append(bpy.context.object)
                if bpy.context.object.type=='MESH' and not bpy.context.object.name[len(child.name)-1]=="C":
                    modelName = bpy.context.object.name
        
        loadedMeshes = []
        loadedColliders = []
        loadedArmatures = []
        loadedActions = []
        modelArmature=""
        modelObjects = []
        modelAnimations = []
        modelCollider = ""
        for child in allObjects:
            # Animation data
            if not child.animation_data==None:
                loadedActions=[]
                for track in child.animation_data.nla_tracks:
                    for strip in track.strips:
                        action=strip.action
                
                        isLoaded = False
                        for act in loadedActions:
                            if act == action.name:
                                isLoaded=True
                                break
                        
                        if isLoaded:
                            continue
                        loadedActions.append(action.name)
                        
                        out = LogPriority(out,WriteAnimation(animationPath,action,child))
                        if child.type=='ARMATURE':
                            modelAnimations.append(action.name)
            
            # Other stuff
            if child.type=='MESH':
                if child.name[len(child.name)-1]=="C":
                    isLoaded=False
                    for l in loadedColliders:
                        if l==child.data.name:
                            isLoaded=True
                    
                    modelCollider=child.data.name
                    
                    if isLoaded:
                        continue
                    
                    loadedColliders.append(child.data.name)
                    
                    out = LogPriority(out,WriteCollider(colliderPath,child))
                        
                else:
                    isLoaded=False
                    for l in loadedMeshes:
                        if l==child.data.name:
                            isLoaded=True
                    
                    modelObjects.append(child)
                    
                    if isLoaded:
                        continue
                    loadedMeshes.append(child.data.name)
                    
                    out = LogPriority(out, WriteMesh(meshPath,child))
                    #print(child)
                    #print(child.data)
                    #print(len(child.data.materials))
                    for mat in child.data.materials:
                        out = LogPriority(out,WriteMaterial(materialPath,mat))
                    
            elif child.type=='ARMATURE':
                isLoaded=False
                for l in loadedArmatures:
                    if l==child.data.name:
                        isLoaded=True
                
                if isLoaded:
                    continue
                
                loadedArmatures.append(child.data.name)
                
                out = LogPriority(out,WriteArmature(armaturePath,child))
                
                modelArmature=child.data.name
                
        if not modelName=="":
            out = LogPriority(out,WriteModel(modelPath,modelName,modelObjects,modelArmature,modelAnimations,modelCollider))   
    else:
        out=["ERROR","Must be in Object Mode!"]
    out[1]=out[1][:-2]
    
    return out

#def LoadFilesA(animPath):
#    out=['INFO','Loaded ']
#    
#    loadedAnims = []
#    
#    if not bpy.context.object==None:
#        if bpy.context.object.mode=="OBJECT":
#            object = bpy.context.object
#            if not object.hide_get():
#                if object.type=="ARMATURE":
#                    if not object.animation_data==None:
#                        if not object.animation_data.action==None:
#                            out = LogPriority(out,WriteAnimation(animPath,object))

#                        
#        else:
#            out=['ERROR',"Switch to object mode!"]
#    else:
#        out=['ERROR',"Select an object!"]
#    
#    out[1]=out[1][:-2]
#    
#    return out

# The code below is for prefererences
class MyProperty(bpy.types.PropertyGroup):
    myGlobal : bpy.props.StringProperty(name="Global Path")
    myMaterial : bpy.props.StringProperty(name="Material Path")
    myMesh : bpy.props.StringProperty(name="Mesh Path")
    myCollider : bpy.props.StringProperty(name="Collider Path")
    myArmature : bpy.props.StringProperty(name="Armature Path")
    myModel : bpy.props.StringProperty(name="Model Path")
    myAnimation : bpy.props.StringProperty(name="Animation Path")
    
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
        
        layout.prop(mexport,"myGlobal")
        layout.prop(mexport,"myMaterial")
        layout.prop(mexport,"myMesh")
        layout.prop(mexport,"myCollider")
        layout.prop(mexport,"myArmature")
        layout.prop(mexport,"myModel")
        layout.prop(mexport,"myAnimation")
        layout.operator("object.mexport_models");

# The code below is for exporting models
class MExportOperator(bpy.types.Operator):
    bl_label = "Export Models"
    bl_idname = "object.mexport_models"
    
    def execute(self, context):
        scene = context.scene
        me = scene.mexport
        
        if me.myGlobal=="":
            me.myGlobal="D:/Development/VisualStudio/ProjectUnknown/ProjectUnknown/assets/"
        if me.myMaterial=="":
            me.myMaterial="materials"
        if me.myMesh=="":
            me.myMesh="meshes"
        if me.myCollider=="":
            me.myCollider="colliders"
        if me.myArmature=="":
            me.myArmature="armatures"
        if me.myModel=="":
            me.myModel="models"
        if me.myAnimation=="":
            me.myAnimation="animations"
        
        glob = me.myGlobal if me.myGlobal[-1]=='/' else me.myGlobal+'/'
        
        message = LoadFiles(
                glob+(me.myModel if me.myModel[-1]=='/' else me.myModel+'/'),
                glob+(me.myAnimation if me.myAnimation[-1]=='/' else me.myAnimation+'/'),
                glob+(me.myMaterial if me.myMaterial[-1]=='/' else me.myMaterial+'/'),
                glob+(me.myMesh if me.myMesh[-1]=='/' else me.myMesh+'/'),
                glob+(me.myCollider if me.myCollider[-1]=='/' else me.myCollider+'/'),
                glob+(me.myArmature if me.myArmature[-1]=='/' else me.myArmature+'/')
        )
        self.report({message[0]}, message[1])
        return {"FINISHED"}
    
classes = [MyProperty,MExportPanel,MExportOperator]

def register():
    for cls in classes:
        bpy.utils.register_class(cls)
        
        bpy.types.Scene.mexport = bpy.props.PointerProperty(type=MyProperty)
 
def unregister():
    for cls in classes:
        bpy.utils.unregister_class(cls)
        
        del bpy.types.Scene.mexport
    
if __name__ == "__main__":
    register()
