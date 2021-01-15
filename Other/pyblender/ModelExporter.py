# Script by Emarioo / Datolsson, Updated 2020-01-08
# Watch Datolsson on youtube for more information
# If any error occurs please contact Datolsson on youtube so I can update this script

bl_info = {
"name": "Model Exporter",
"category": "Object",
"author": "Datolsson",
"description": "Menu and panel for turning blender models, animations and armatures into data files"
}

import bpy
import struct
import math
from mathutils import Matrix
from mathutils import Vector

#D:/Development/VisualStudioProjects/Reigai-Dimension/Reigai-Dimension/assets/

# Pressing Export Models
#  In Selection (multiple meshes, bones, armatures)
#   Loop through collection and Load Object and it's children
#  Loading Object
#   If object's name == data's name then make an own model
#   Else if names are different then don't make model and only make a mesh and dump into existing model. 
#    If no collection then show warning that object has no collection
#   data is a mesh and has not been load this time then load that mesh
#  Loading Mesh
#   if name of mesh exist then don't load
#   
#  Custom animatin groups

# How To use
#   Select an object in OBJECT mode and run the script to generate files
#
# Notes
#   Supported Interpolation: Beizer, Linear, Constant
#   C at the end of a mesh to make it a collider.
#   Collider Mesh only supports quad polygons
#   The only currently supported rotation type is Euler ZYX.
#   Remember to use the right action name. It might contain ...Action.001
#   Consider recalculating normals if they are wierd
#
#
# Check out this link if you want to change quaternions to euler rotation:
#   https://blender.stackexchange.com/questions/40711/how-to-convert-quaternions-keyframes-to-euler-ones-in-several-actions
#
# --- ISSUES --- 
#   Improved support for modifiers?
#   Blender has 3 Vertex Colors for every triangle while my game has one vertex color for every vertex
#    what happens when one vertex uses different vertex colors?
#   Support for Quaternions
#   Issue when renaming things like modifiers...
#   More Material support
#   Filter unused FCurves

# Change the code below at your own risk!

# Used to switch the axis (Y-up instead of Z-up)
axisConvert = Matrix((
(1.0, 0.0, 0.0, 0.0),
(0.0, 0.0, 1.0, 0.0),
(0.0,-1.0, 0.0, 0.0),
(0.0, 0.0, 0.0, 1.0)))
        
def MeshFile(path,original,object):
    out=['INFO',original.name]
    # Create File
    file = open(path+original.name+".mesh","wb")# CHANGED
    
    # Determine what type of mesh
    type=0 # 0=VertexColor  1=UV  2=VertexColor with weights  3=UV with weights 
    singleColor=[1,1,1]
    
    triC=0
    for poly in object.data.polygons:
        if len(poly.vertices)==4:
            triC=triC+2
        else:
            triC=triC+1
    useWeight=False
    for mod in object.modifiers:
        if mod.name=="Armature":
            useWeight=True
            type=type+2
            break
    
    useTexture=False
    textureName=""
    useVertexColor=False
    if not object.active_material==None:
        if object.active_material.use_nodes:
            base = object.active_material.node_tree.nodes["Material Output"].inputs["Surface"].links[0].from_node.inputs["Base Color"]
            if len(base.links)>0:
                if base.links[0].from_node.name=="Image Texture":
                    type=type+1
                    useTexture=True
                    tempName = base.links[0].from_node.image.name
                    textureName = tempName[0:len(tempName)-4]
                elif base.links[0].from_node.name=="Vertex Color":
                    useVertexColor=True;
            else:
                singleColor = object.active_material.diffuse_color
        else:
            singleColor = object.active_material.diffuse_color
    
    # Acquire Data
    verC=len(object.data.vertices)
    
    uniqueColor=[]
    indexColor=[]
    if useTexture:
        for u in object.data.uv_layers.active.data:
            same = False
            for i in range(0,len(uniqueColor)):
                if uniqueColor[i][0]==u.uv[0] and uniqueColor[i][1]==u.uv[1]:
                    same=True
                    indexColor.append(i)
                    break
            
            if not same:
                indexColor.append(len(uniqueColor))
                uniqueColor.append([u.uv[0],u.uv[1]]);
                #file.write(struct.pack("=ff",u.uv[0],u.uv[1]))
                
    elif useVertexColor:
        for c in object.data.vertex_colors.active.data:
            same = False
            for i in range(0,len(uniqueColor)):
                if uniqueColor[i][0]==c.color[0] and uniqueColor[i][1]==c.color[1] and uniqueColor[i][2]==c.color[2]:
                    same=True
                    indexColor.append(i)
                    break
                
            if not same:
                indexColor.append(len(uniqueColor))
                uniqueColor.append([c.color[0],c.color[1],c.color[2]]);
    else:
        indexColor.append(len(uniqueColor))
        uniqueColor.append([singleColor[0],singleColor[1],singleColor[2]]);
    
    colorC=len(uniqueColor)
    
    uniqueWeight=[]
    indexWeight=[]
    if useWeight:
        for ve in object.data.vertices:
            weight=[0,0,0,0,0,0]
            # Grab the most important weights
            for gr in ve.groups:
                lowW=gr.weight
                lowB=gr.group
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
    file.write(struct.pack("=B",type))
    file.write(struct.pack("=H",verC))# Vertex Count
    file.write(struct.pack("=H",colorC))
    if useTexture:
        file.write(struct.pack("=B",len(textureName)))
        file.write(bytearray(textureName,"UTF-8"))
    if useWeight:
        file.write(struct.pack("=H",weightC))
    file.write(struct.pack("=H",triC))# Triangle Count (Maximum 65536)
    
    # Write Mesh Data
    for ver in object.data.vertices:
        v = Vector((ver.co[0],ver.co[1],ver.co[2],0))
        if original.modifiers.find("Armature")==-1:
            v=axisConvert@v
        file.write(struct.pack("=fff",v[0],v[1],v[2]))# SWITCH Y Z
    
    for c in uniqueColor:
        if useTexture:
           file.write(struct.pack("=ff",c[0],c[1]))
        else:
            file.write(struct.pack("=fff",c[0],c[1],c[2]))
    
    if useWeight:
        for w in uniqueWeight:
            leng=w[3]+w[4]+w[5]
            file.write(struct.pack("=BBBfff",w[0],w[1],w[2],w[3]/leng,w[4]/leng,w[5]/leng))
  
    # Write index information for Triangles
    for poly in object.data.polygons:
        v = poly.vertices
        lo = poly.loop_indices
        if type<2:
            if len(v)==3:
                l=[0,0,0]
                if useTexture or not colorC==1:
                    l=[indexColor[lo[0]],indexColor[lo[1]],indexColor[lo[2]]]
                file.write(struct.pack("=HHHHHH",v[1],l[1],v[2],l[2],v[0],l[0]))
            elif len(v)==4:
                l=[0,0,0,0]
                if useTexture or not colorC==1:
                    l=[indexColor[lo[0]],indexColor[lo[1]],indexColor[lo[2]],indexColor[lo[3]]]
                file.write(struct.pack("=HHHHHH",v[1],l[1],v[2],l[2],v[0],l[0]))
                file.write(struct.pack("=HHHHHH",v[2],l[2],v[3],l[3],v[0],l[0]))
            else:
                out=['WARNING',"Triangulate failed in "+original.name]
                #print("Cannot have more than five vertices per quad!")
        else:
            if len(v)==3:
                l=[0,0,0]
                if useTexture or not colorC==1:
                    l=[indexColor[lo[0]],indexColor[lo[1]],indexColor[lo[2]]]
                w=[indexWeight[v[0]],indexWeight[v[1]],indexWeight[v[2]]]
                file.write(struct.pack("=HHHHHHHHH",v[1],l[1],w[1],v[2],l[2],w[2],v[0],l[0],w[0]))
            elif len(v)==4:
                l=[0,0,0,0]
                if useTexture or not colorC==1:
                    l=[indexColor[lo[0]],indexColor[lo[1]],indexColor[lo[2]],indexColor[lo[3]]]
                w=[indexWeight[v[0]],indexWeight[v[1]],indexWeight[v[2]],indexWeight[v[3]]]
                file.write(struct.pack("=HHHHHHHHH",v[1],l[1],w[1],v[2],l[2],w[2],v[0],l[0],w[0]))
                file.write(struct.pack("=HHHHHHHHH",v[2],l[2],w[2],v[3],l[3],w[3],v[0],l[0],w[0]))
            else:
                out=['WARNING',"Triangulate failed in "+original.name]
                #print("Cannot have more than five vertices per quad!")
    
    # Cleanup
    file.close()
    return out

def AnimFile(path,object):
    action=object.animation_data.action
    out = ['INFO', action.name]
    # Create file
    file = open(path+action.name+".anim","wb")

    # Write Anim Information
    fstart = int(action.frame_range[0])
    fend = int(action.frame_range[1])

    file.write(struct.pack("=H",fstart))
    file.write(struct.pack("=H",fend))
    file.write(struct.pack("=f",24)) # Frame per second
    
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

    # Write Data
    xyz="XYZ"
    for group in action.groups:
        if group.name=="Object Transforms":
            file.write(struct.pack("=I",-1))
        else:
            id = object.data.bones.find(group.name)
            if id==-1:
                # group name not found
                continue
            else:
                file.write(struct.pack("=I",id))
           
        curveInt=0
        curves=[0,0,0,0,0,0,0,0,0]
        for curve in group.channels:
            if curve.is_empty or curve.mute or not curve.is_valid:# is this safe?
                continue
            
            con=0
            if curve.data_path[-8:]=="location":
                con=0
            elif curve.data_path[-5:]=="scale":
                con=3
            elif curve.data_path[-10:]=="quaternion":
                con=6
            elif curve.data_path[-5:]=="euler":
                con=3#print("Euler for rotation is not allowed please change it to Quaternions. Check python script for more information.")
            
            con=con+curve.array_index
            # Code used to switch z and y
#            if con>5:
#                if curve.array_index==2:
#                    con=con+1
#                elif curve.array_index==3:
#                    con=con-1
#            else:
#                if curve.array_index==1:
#                    con=con+1
#                elif curve.array_index==2:
#                    con=con-1
            
            curves[con]=1
                    
        for i in range(0,9):
            curveInt += curves[i]*pow(2,i);
            
        file.write(struct.pack("=H",curveInt));
        
        for i in range(0,9):
            
            if curves[i]==0:
                continue
            
            for curve in group.channels:
                
                con=0
                if curve.data_path[-8:]=="location":
                    con=0
                elif curve.data_path[-5:]=="scale":
                    con=3
                elif curve.data_path[-10:]=="quaternion":
                    con=6
                elif curve.data_path[-5:]=="euler":
                    con=3
                
                # Switch y and z
                ind=curve.array_index
                # Code used to switch z and y
#                if con>5:
#                    if curve.array_index==2:
#                        ind=ind+1
#                    elif curve.array_index==3:
#                        ind=ind-1
#                else:
#                    if curve.array_index==1:
#                        ind=ind+1
#                    elif curve.array_index==2:
#                        ind=ind-1
                        
                con=con+ind
                 
                if not con==i:
                        continue
                
                file.write(struct.pack("=H",len(curve.keyframe_points)))
                
                for key in curve.keyframe_points:
                    pol=key.interpolation
                    if pol=="LINEAR":
                        pol="L"
                    elif pol=="BEZIER":
                        pol="B"
                    else:
                        pol="C"
                    
                    file.write(struct.pack("=sHf",bytearray(pol,"UTF-8"),int(key.co[0]),key.co[1])) 
    
    # Cleanup
    file.close()
    return out

def BoneFile(path, original, temp):
    out = ['INFO',original.name]
    # Creating file
    boneFile = open(path+original.name+".bone","wb")
    
    # Loading data
    boneCount = len(temp.data.bones)
    boneFile.write(struct.pack("=B",boneCount))
    
    for bone in temp.data.bones:
        index = temp.data.bones.find(bone.parent.name) if bone.parent else 0
        
        # axisConvert to switch Y-axis and Z-axis
        #  applying axisConvert to root bone will apply it to all other bones as well
        local_matrix = axisConvert@bone.matrix_local
        if bone.parent:
            local_matrix = (bone.parent.matrix_local).inverted() @ bone.matrix_local
       
        local_matrix = local_matrix.transposed()
        inv_model_matrix = bone.matrix_local.inverted().transposed()
        
        boneFile.write(struct.pack("=I",index))
        boneFile.write(struct.pack("=16f",*local_matrix[0],*local_matrix[1],*local_matrix[2],*local_matrix[3]))
        boneFile.write(struct.pack("=16f",*inv_model_matrix[0],*inv_model_matrix[1],*inv_model_matrix[2],*inv_model_matrix[3]))
      
    # Cleanup
    boneFile.close()
    return out

def CollFile(path,original,object):
    out=['INFO',original.name]
    # Create File
    file = open(path+original.name+".coll","wb")
    
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
    
    return out

def LoadFilesM(meshPath,collPath,bonePath):
    out=['INFO','Loaded ']
    
    loadedMeshes = []
    
    if bpy.context.object.mode=="OBJECT":
        for o in bpy.context.selected_objects:
            
    else:
        out=['ERROR',"Switch to object mode!"]
    
    if not bpy.context.object==None:
        if bpy.context.object.mode=="OBJECT":
            bpy.ops.object.select_all(action="DESELECT")
            objects = [bpy.context.object]
            for o in bpy.context.object.children:
                objects.append(o)
          
            for object in objects:
                if not object.hide_get():
                    if object.type=="MESH" or object.type=="ARMATURE":
                        collider = True if object.name[len(object.name)-1]=="C" else False
                        
                        # Make a copy of the object and apply modifiers
                        temp=object.copy()
                        temp.data=object.data.copy()
                        bpy.context.scene.collection.objects.link(temp)
                        temp.select_set(True)
                        bpy.context.view_layer.objects.active=temp
                        
                        message = ['NONE',""]
                        
                        # Create file
                        if object.type=="MESH":
                            if collider:
                                for mod in temp.modifiers:
                                    bpy.ops.object.modifier_apply(modifier=mod.name)
                                        
                                message = CollFile(collPath,object,temp)
                               
                            else:
                                if temp.modifiers.find("Triangulate")==-1:
                                    temp.modifiers.new("Triangulate",'TRIANGULATE')
                                    temp.modifiers["Triangulate"].quad_method="SHORTEST_DIAGONAL"
                            
                                for mod in temp.modifiers:
                                    bpy.ops.object.modifier_apply(modifier=mod.name)
                                        
                                message = MeshFile(meshPath,object,temp)
                            
                        elif object.type=="ARMATURE":
                            # Read Animation
                            if not object.animation_data==None:
                                if not object.animation_data.action==None:
                                    message = AnimFile(animPath,object)
                                    if out[0]=='INFO':
                                        if message[0]=='INFO':
                                            out[1]=out[1]+message[1]+", "
                                        else:
                                            out=message
                                    elif out[0]=='WARNING':
                                        if message[0]=='WARNING':
                                            out[1]=out[1]+message[1]+", "
                                        elif message[0]=='ERROR':
                                            out=message
                                    elif out[0]=='ERROR':
                                        if message[0]=='ERROR':
                                            out[1]=out[1]+message[1]+", "
                                        
                            for mod in temp.modifiers:
                                bpy.ops.object.modifier_apply(modifier=mod.name)
                            
                            message = BoneFile(bonePath,object,temp)
                        
                        if out[0]=='INFO':
                            if message[0]=='INFO':
                                out[1]=out[1]+message[1]+", "
                            else:
                                out=message
                        elif out[0]=='WARNING':
                            if message[0]=='WARNING':
                                out[1]=out[1]+message[1]+", "
                            elif message[0]=='ERROR':
                                out=message
                        elif out[0]=='ERROR':
                            if message[0]=='ERROR':
                                out[1]=out[1]+message[1]+", "
                            
                        bpy.ops.object.delete()
        else:
            out=['ERROR',"Switch to object mode!"]
    else:
        out=['ERROR',"Select an object!"]
    
    out[1]=out[1][:-2]
    
    return out

# The code below is for prefererences
class MyProperty(bpy.types.PropertyGroup):
    myGlobal : bpy.props.StringProperty(name="Global Path")
    myMesh : bpy.props.StringProperty(name="Mesh Path")
    myColl : bpy.props.StringProperty(name="Coll Path")
    myBone : bpy.props.StringProperty(name="Bone Path")
    myAnim : bpy.props.StringProperty(name="Anim Path")
    
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
        layout.prop(mexport,"myMesh")
        layout.prop(mexport,"myColl")
        layout.prop(mexport,"myBone")
        layout.prop(mexport,"myAnim")
        layout.operator("object.mexport_models");
        layout.operator("object.mexport_animations");

# The code below is for exporting models
class MExportOperatorM(bpy.types.Operator):
    bl_label = "Export Models"
    bl_idname = "object.export_models"
    
    def execute(self, context):
        scene = context.scene
        me = scene.mexport
        
        if me.myGlobal=="":
            me.myGlobal="D:/Development/VisualStudioProjects/Reigai-Dimension/Reigai-Dimension/assets/"
        if me.myMesh=="":
            me.myMesh="meshes"
        if me.myColl=="":
            me.myColl="colliders"
        if me.myBone=="":
            me.myBone="bones"
        
        glob = me.myGlobal if me.myGlobal[-1]=='/' else me.myGlobal+'/'
        message = LoadFilesM(
                glob+(me.myMesh if me.myMesh[-1]=='/' else me.myMesh+'/'),
                glob+(me.myColl if me.myColl[-1]=='/' else me.myColl+'/'),
                glob+(me.myBone if me.myBone[-1]=='/' else me.myBone+'/')
        )
        self.report({message[0]}, message[1])
        return {"FINISHED"}
    
#This code is for animations
class MExportOperatorA(bpy.types.Operator):
    bl_label = "Export Models"
    bl_idname = "object.mexport_animations"
    
    def execute(self, context):
        scene = context.scene
        me = scene.mexport
        # If directorys are empty
        #D:/Development/VisualStudioProjects/Reigai-Dimension/Reigai-Dimension/assets/
        if me.myGlobal=="":
            me.myGlobal="D:/Development/VisualStudioProjects/Reigai-Dimension/Reigai-Dimension/assets/"
        if me.myAnim=="":
            me.myAnim="animations"
            
        glob = me.myGlobal if me.myGlobal[-1]=='/' else me.myGlobal+'/'
        message = LoadFilesA(
                glob+(me.myAnim if me.myAnim[-1]=='/' else me.myAnim+'/')
        )
        self.report({message[0]}, message[1])
        return {"FINISHED"}
    
classes = [MyProperty,MExportPanel,MExportOperatorM,MExportOperatorA,MExportMenu]

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