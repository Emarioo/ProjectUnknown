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
import traceback
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


# Used to switch the axis (Y-up instead of Z-up)

from bpy_extras.io_utils import axis_conversion

# WriteAnimation can't use this matrix, so it switches z and y manually.
axisConvert = axis_conversion(from_forward="-Y",from_up="Z",to_forward="Z",to_up="Y").to_4x4()
axisIConvert = axisConvert.inverted()

class ExportLog:
    def __init__(self):
        self.errors=[]
        self.warnings=[]
        self.infos=[]
        
    def print(self,type,msg):
        if type=="ERROR":
            self.errors.append(msg)
        elif type=="WARNING":
            self.warnings.append(msg)
        elif type=="INFO":
            self.infos.append(msg)
    
    def getMessage(self):
        lvl = self.getLevel()
        
        msgs = None
        if lvl=="ERROR":
            msgs = self.errors
        elif lvl=="WARNING":
            msgs = self.warnings
        elif lvl=="INFO":
            msgs = self.infos
        
        msg=""
        for i in range(0,len(msgs)):
            if not i==0:
                msg+=", "
            msg+=msgs[i]
        return msg
        
    def getLevel(self):
        if len(self.errors):
            return "ERROR"
        if len(self.warnings):
            return "WARNING"
        return "INFO"

# File functions
binaryForm=False
class FileWriter:
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
    
def WriteMesh(log,file,object,isBoned,originName):

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
    if isBoned:
        for ve in object.data.vertices:
            weight=[0,0,0,0,0,0]
            # Grab the most important weights
            for gr in ve.groups:
                lowW=gr.weight
                # object.modifiers <- original.mod...
                lowB=object.modifiers["Armature"].object.data.bones.find(object.vertex_groups[gr.group].name)
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
    file.writeUChar(1 if isBoned else 0)
    file.writeComment("Position Count")
    file.writeUShort(verC)# Vertex Count (Maximum 65536)
    file.writeComment("Color Count")
    file.writeUShort(colorC)
    
    file.writeComment("Materials")
    file.writeUChar(len(object.data.materials))
    for mat in object.data.materials:
        file.writeString(mat.name)
        
    if isBoned:
        file.writeComment("Weight Count")
        file.writeUShort(weightC)
    file.writeComment("Triangle count")
    file.writeUShort(triC)# Triangle Count (Maximum 65536)
    #print("Points "+str(verC)+" Textures "+str(colorC)+" Triangles" +str(triC)+" Mats "+str(len(object.data.materials)))
    
    # Write Mesh Data
    file.writeComment("Positions")
    for ver in object.data.vertices:
        v = Vector((ver.co[0],ver.co[1],ver.co[2],0))
        #v = Vector((ver.co[0],ver.co[2],-ver.co[1],0))
        #if not useArmature:
        #    v = Vector((ver.co[0],ver.co[1],ver.co[2],0))
        file.writeFloat(v[0],v[1],v[2])# SWITCH Y Z
    
    file.writeComment("Colors")
    for c in uniqueColor:
        file.writeFloat(c[0],c[1],c[2])
       # file.writeFloat(c[0],c[1],c[2])
            
    if isBoned:
        file.writeComment("Weights")
        for w in uniqueWeight:
            leng=w[3]+w[4]+w[5]
            file.writeUChar(w[0],w[1],w[2])
            file.writeFloat(w[3]/leng,w[4]/leng,w[5]/leng)
  
    # Write index information for Triangles
    file.writeComment("Triangles")
    for poly in object.data.polygons:
        v = poly.vertices
        lo = poly.loop_indices
        if not isBoned:
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
                log.print('WARNING',"Triangulate failed in "+originName)
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
                log.print('WARNING',"Triangulate failed in "+originName)
                #print("Cannot have more than five vertices per quad!")
    
    log.print('INFO',originName)

def WriteAnimation(log, file, object, action):
    
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
            
            if con%3==1 and not curve.array_index==3: # y to z
                con+=1
            elif con%3==2:# z to y
                con-=1
            
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
                
                if con%3==1 and not curve.array_index==3: # y to z
                    con+=1
                elif con%3==2:# z to y
                    con-=1
                
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
                    
                    if con%3==2: # -y to z
                        file.writeFloat(-key.co[1])
                    else:
                        file.writeFloat(key.co[1])
    
    log.print('INFO',action.name)

def WriteMaterial(log,file,material):
    
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
    
    log.print('INFO',material.name)

def WriteArmature(log, file, object, originName):
    
    # Loading data
    file.writeComment("Bone count")
    file.writeUChar(len(object.data.bones))
    file.writeComment("Bone data")
    for bone in object.data.bones:
        parentIndex = object.data.bones.find(bone.parent.name) if bone.parent else 0 # the first bone will not use the parent index

        localMatrix=bone.matrix_local
        invModelMatrix = localMatrix.inverted()
        
        if bone.parent:
            parentMatrixLocal = bone.parent.matrix_local
            localMatrix = parentMatrixLocal.inverted() @ localMatrix
        
        # already done on object
        #localMatrix=axisConvert@localMatrix@axisIConvert
        #invModelMatrix=axisConvert@invModelMatrix@axisIConvert
        
        localMatrix = localMatrix.transposed()
        invModelMatrix = invModelMatrix.transposed()
        
        file.writeUShort(parentIndex)
        file.writeMatrix(localMatrix)
        file.writeMatrix(invModelMatrix)
      
    log.print('INFO',originName)

def GetColliderType(object):
    if object.data.name[:4]=="CUBE":
        return "CUBE"
    elif object.data.name[:6]=="SPHERE":
        return "SPHERE"
    elif object.data.name[:3]=="MAP":
        return "MAP"
    elif object.data.name[:7]=="CAPSULE":
        return "CAPSULE"
    print('ERROR',"Invalid collider type "+str(object.data.name))
    return False

def GetMedianPoint(data):
    median = Vector()
    for v in data.vertices:
        median+=v.co
    median/=len(data.vertices)
    return median

def CalculateBounds(data):
    minBound = Vector()
    maxBound = Vector()
    for v in data.vertices:
        for i in range(0,3):
            if v.co[i]<minBound[i]:
                minBound[i]=v.co[i]
            if v.co[i]>maxBound[i]:
                maxBound[i]=v.co[i]
    
    return maxBound-minBound, (maxBound+minBound)/2

# function will NOT change original, z is up
def DecideColliderType(object):
    data=object.data
    
    type=""
    finished=False
    
    if len(data.vertices)==0:
        finished=True
        #print("Empty")
    
    #median = GetMedianPoint(data)
    
    bounds, center = CalculateBounds(data)

    ## ISSUE: vertices not included in polygons should invalidate the collider because a collision happens on a plane.
    ##      some colliders require that a mesh is connected even polygons need to be connected
    ## NOTE: mesh cannot be rotated. cube, capsule, heightmap needs to be aligned to the axis.
    
    #### check cube
    if not finished:
        if len(data.vertices)==8:
            # A cube or block requires 90 deg. angles
            #print("Polygons:",len(data.polygons))
            tolerance=0.001
            failed = False
            
            # axis aligned
            for p in data.polygons:
                count = len(p.vertices)
                for i in range(0,count):
                    a = data.vertices[p.vertices[(i-1)%count]].co
                    b = data.vertices[p.vertices[i]].co
                    c = data.vertices[p.vertices[(i+1)%count]].co
                    
                    ab = a-b
                    bc = b-c
                    
                    ang = ab.dot(bc)
                    #print(ang)
                    if math.fabs(ang)>tolerance:
                        failed=True
                        break
                
                if failed:
                    break
            
            if not failed:
                aligns=0
                for v in data.vertices:
                    f = data.vertices[0]
                    #print(v,f)
                    if v==f:
                        continue
                    
                    if v.co.x==f.co.x or v.co.y==f.co.y or v.co.z==f.co.z:
                        aligns+=1
                        #print(v.co,f.co)
                
                #print("Aligns:",aligns)
                if aligns<3:
                    finished=True # mesh is a cube but it is rotate
                else:
                    type="CUBE"
                    finished=True
        
    #### Height map
    if not finished:
        # ISSUE: Polygons should not be allowed to be disconnected
        
        # NOTE: map is not affected by original's scale
        tolerance=0.001
        failed=False
        for p in data.polygons:
            count = len(p.vertices)
            for i in range(0,count):
                a = data.vertices[p.vertices[(i-1)%count]].co
                b = data.vertices[p.vertices[i]].co
                c = data.vertices[p.vertices[(i+1)%count]].co
                
                ab = a-b
                bc = b-c
                
                ang = ab.x*bc.x+ab.z*bc.z
                #print(ang)
                if math.fabs(ang)>tolerance:
                    #print("Bad angle:",ang)
                    failed=True
                    break
            
            if failed:
                break
        
        if not failed:
            aligns=0
            p0 = data.polygons[0]
            
            for i in p0.vertices:
                v = data.vertices[i]
                f = data.vertices[p0.vertices[0]]
                #print(v.co,f.co)
                if v==f:
                    continue
                
                if v.co.x==f.co.x or v.co.z==f.co.z:
                    aligns+=1
                    #print(v.co,f.co)
                
            #print("Aligns:",aligns)
            if aligns<2:
                finished=True
            else:
                
                # there can only be 4 vertices which are connected to 2 edges
                refs=[0]*len(data.vertices)
                for key in data.edge_keys:
                    refs[key[0]]+=1
                    refs[key[1]]+=1
                
                vertWith2=0
                for val in refs:
                    if val==2:
                        vertWith2+=1
                        if vertWith2==5:
                            break
                        
                if vertWith2>4:
                    finished=True
                else:
                    type="MAP"
                    finished=True
    
    #### check sphere
    if not finished:
        # Sphere is determined by radius. Because of this, a cube will be seen as a sphere. Check if cube before 
        minRadius=9999999
        maxRadius=-1
        for mv in data.vertices:
            d = mv.co-center
            radius = d.x*d.x+d.y*d.y+d.z*d.z
            if radius<minRadius:
                minRadius=radius
            if radius>maxRadius:
                maxRadius=radius
        
        minRadius = math.sqrt(minRadius)
        maxRadius = math.sqrt(maxRadius)
        
        
        tolerance=0.05
        radiusDiff = math.fabs(maxRadius-minRadius)
        #print(radiusDiff)
        if radiusDiff<tolerance:
            type="SPHERE"
            finished=True
            #print(radiusDiff)
        
    #### check capsule    
    if not finished:
        ## uv sphere isn't 100% spherical
        
        #dim = object.dimensions.copy()
        # scaling
        #s=object.scale
        
        radius2 = (bounds.x+bounds.z)/4
        radius = math.sqrt(radius2)
        height = bounds.y
        
        #print("mid",median)
        top = center.copy()
        top.y+=height/2-radius
        bottom = center.copy()
        bottom.y+=-height/2+radius
        
        tolerance = 0.001
        
#        print("Dim",bounds)
#        print("R/H:",radius,height,center)
#        print("Top",top,"Bot",bottom)
        
        failed=False
        for mv in data.vertices:
            v = mv.co.copy()
            d = top-v
            rt = d.x*d.x+d.y*d.y+d.z*d.z
            #print("v",v)
            d = bottom-v
            rb = d.x*d.x+d.y*d.y+d.z*d.z
            
            #print(d)
            #print(rt,rb)
            
            diffT = math.fabs(radius2-rt)
            diffB = math.fabs(radius2-rb)
            if diffT>tolerance and diffB>tolerance:
                #print("Diffs:",diffT,diffB)
                failed=True
                break
            
        if not failed:
            type="CAPSULE"
            finished=True
    
    if finished:
        #print("Collider type:",type)
        pass
    else:
        print("Collider type: Unknown")
        
    return type

def rounding(x):
    return round(x*1000)/1000

def WriteCollider(log,file,object,colliderType,originName):

    SPHERE = 0
    CUBE = 1
    CAPSULE = 2
    MAP = 3
    
    bounds, center = CalculateBounds(object.data)
    
    file.writeComment("Type")
    if colliderType=="CUBE":
        file.writeUChar(CUBE)
        file.writeComment("Size")
        # dimensions is used here to get the size of the cube. Scale would give the wrong value.
        file.writeFloat(bounds[0]/2,bounds[1]/2,bounds[2]/2)
        
        log.print('INFO',originName)
    elif colliderType=="SPHERE":
        file.writeUChar(SPHERE)
        file.writeComment("radius")
        s = bounds/2
#        for i in range(0,3):
#            s/=object.scale[i]
        radius = math.sqrt(s.x*s.x+s.y*s.y+s.z*s.z)
        file.writeFloat(radius);
#        file.writeComment("Position")
#        offset = GetMedianPoint(object.data)
#        file.writeFloat(offset.x,offset.y,offset.z)
        
        log.print('INFO',originName)
    elif colliderType=="MAP":
        # HEIGHT MAP ASSUMES SOME STUFF
        # Y is up, this means the map needs to be on it's side
        file.writeUChar(MAP)
        minHeight=9999999
        maxHeight=-9999999
        for v in object.data.vertices:
            if v.co.y>maxHeight:
                maxHeight=v.co.y
            if v.co.y<minHeight:
                minHeight=v.co.y
        
        # mesh can be assumed to be a grid, otherwise the type wouldn't be considered a map
        # starts with one to account for the base
        columns = 0
        rows = 0
        base = object.data.vertices[0].co
        
        for i in range(0,len(object.data.vertices)):
         
            v = object.data.vertices[i].co
            
            if v.x==base.x: # x axis means row
                rows+=1
            if v.z==base.z:
                columns+=1
        
        #print("W/H/S",columns,rows,len(object.data.vertices))
        if not len(object.data.vertices)==columns*rows:
            log.print('WARNING',"Height map size was miscalculated!")
        else:
            #median = GetMedianPoint(object.data);
                
            center.x=rounding(center.x)
            center.y=rounding(center.y)
            center.z=rounding(center.z)
           
            if not center.x==0 or not center.z == 0:
                print("CenterPoint:",center)
                log.print('WARNING',"Height map is not centered!")
                
            file.writeComment("Columns")
            file.writeUShort(columns)
            file.writeComment("Rows")
            file.writeUShort(rows)
            
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
            file.writeComment("CellSize")
            cellSize = Vector()
            cellSize.x = bounds.x/(columns-1)
            cellSize.y = 1
            cellSize.z = bounds.z/(rows-1)
            #print("Scale: ",scaleX,scaleY,scaleZ)
            file.writeFloat(cellSize.x,cellSize.y,cellSize.z)
            file.writeComment("Height Data")
        
             # The vertices are not stored in order, which means i need to do that.
            heights = [0]*len(object.data.vertices)
  
            baseX = bounds.x/2
            baseZ = bounds.z/2
            #print("BaseXZ: ",baseX,baseZ," Width: ",width)
            for v in object.data.vertices:
                xi = round((baseX+v.co.x)/cellSize.x)
                zi = round((baseZ+v.co.z)/cellSize.z)
                #print(xi,zi)
                # index should hopefully be less than len(data), if it's not, it's a bug.
                index = zi*columns+xi
                #print("XY: ",v.co.x,v.co.z," Index: ",xi,zi)
                    
                heights[index]=v.co.y
            
           #for val in data:
           #     file.writeFloat(val)
            file.writeFloat(*heights)
            
            log.print('INFO',originName)
    
    elif colliderType=="CAPSULE":
        file.writeUChar(CAPSULE)
        file.writeComment("Radius")
        radius2 = (bounds.x+bounds.z)/4
        radius = math.sqrt(radius2)
        file.writeFloat(radius)
        file.writeComment("Height")
        file.writeFloat(bounds.y)
        
        log.print('INFO',originName)
        
    elif colliderType=="MESH":
#        file.writeUChar(2)
#        
#        furthest=0
#        for v in object.data.vertices:
#            dist=v.co.x*v.co.x+v.co.y*v.co.y+v.co.z*v.co.z
#            if dist>furthest:
#                furthest=dist
#        furthest=math.sqrt(furthest)
#        
#        triCount=0
#        for poly in object.data.polygons:
#            if len(poly.vertices)==3:
#                triCount+=1
#        
#        file.writeComment("Vertex count")
#        file.writeUShort(len(object.data.vertices))
#        file.writeComment("Triangle count")
#        file.writeUShort(triCount)
#        file.writeComment("Furthest point")
#        file.writeFloat(furthest)
#        
#        # Write Points
#        for ver in object.data.vertices:
#            v = ver.co
#            file.writeFloat(v[0],v[1],v[2])
#            #file.write(struct.pack("=fff",v[0],v[1],v[2]))
#        
#        # Write Planes
#        for poly in object.data.polygons:
#            v = poly.vertices
#            if len(v)==3:
#                file.writeUShort(v[0],v[1],v[2])
        
        log.print('INFO',originName)
    else:
        if not colliderType:
            colliderType="Unknown"
        log.print('WARNING',"Collider "+colliderType+" is not supported!")

def WriteModel(log,file,instances,animations,originName):
    
    instCopy = instances.copy()

    sort = []
    
    # add the root, should be the first one but this might change in the future so better be safe
    for inst in instances:
        if inst.object.parent==None:
            sort.append(inst)
            instCopy.remove(inst)
            break
        
    index = 0
    limit = len(instCopy)*len(instCopy) # safety feature
    while len(instCopy)>0 and limit>0:
        #print("index ",index)
       
        i=0
        while i<len(instCopy) and limit>0:
            inst = instCopy[i]
            
            if inst.object.parent==None:
                continue    
            
            parIndex = -1
            for j in range(0,len(sort)):
                if inst.object.parent==sort[j].object:
                    parIndex=j
                    break
            
            #print(" ",parIndex)
            if index == parIndex:
                sort.append(inst)
                instCopy.remove(inst)
                i-=1
            
            i+=1
            limit-=1
        
        index+=1
        limit-=1
    #Logging("WARNING","Instance sorting")
    
    #print(sort)
    
    file.writeComment("Instance count")
    file.writeUChar(len(sort))

    for inst in sort:
        parentIndex = -1
        for j in range(0,len(sort)):
            if inst.object.parent==sort[j].object:
                parentIndex=j
                break
        
        localMatrix=inst.object.matrix_local
        #if parentIndex==-1:
        localMatrix=axisConvert@localMatrix@axisIConvert
        
        
        #if parentIndex==-1:
            #local_matrix = local_matrix.Translation(local_matrix[3])
        
        type = 0
        if inst.asset.type=="MESH":
            type=0
        elif inst.asset.type=="ARMATURE":
            type=1
        elif inst.asset.type=="COLLIDER":
            type=2
        
        # instance name
        file.writeString(inst.object.name)
        #file.writeComment("Instance Type")
        file.writeUChar(type)
        
        # data name (asset name)
        file.writeString(inst.asset.name)
            
        file.writeShort(parentIndex)
        localMatrix=localMatrix.transposed()
        file.writeMatrix(localMatrix)
    
    file.writeComment("Animation count")
    file.writeUShort(len(animations))
    for asset in animations:
        file.writeString(asset.action.name)
    
    log.print('INFO',originName)

class Asset:
    type=""
    object=None
    action=None
    name=""
    copy=None
    
    def __init__(self,type,object,action=None):
        self.type=type
        self.object=object
        self.action=action
    
class Instance:
    asset=None
    object=None
    
    def __init__(self,asset,object):
        self.asset=asset
        self.object=object

def ExportAssets(log,assetPath):
    if not bpy.context.mode=='OBJECT':
        log.print('ERROR',"Must be in object mode!")
        return
    elif len(bpy.context.selected_objects)==0:
        log.print('ERROR',"Select an object!")
        return

    activeObject = bpy.context.object
    selectedObjects = bpy.context.selected_objects.copy()
    
    for object in selectedObjects: 
        if not object.parent==None:
            log.print("ERROR","Selected object cannot be a child!")
            return
    
    for root in selectedObjects: 
        modelPath=assetPath+root.name+"\\"
            
        os.makedirs(os.path.dirname(modelPath),exist_ok=True)
        
        bpy.ops.object.select_all(action="DESELECT")
        
        meshes = []
        armatures = []
        colliders = []
        animations = []
        materials = []
        
        instances = []
        
        remaining=[root]
        while len(remaining)>0:
            object=remaining[0]
            remaining.remove(object)
            
            # This bit will ignore hidden objects
            if object.hide_get():
                continue
            
            for o in object.children: 
                remaining.append(o)
            
            # Write animation
            if not object.animation_data==None:
                for track in object.animation_data.nla_tracks:
                    for strip in track.strips:
                        action=strip.action
                
                        found = None
                        for asset in animations:
                            if asset.action.name == action.name:
                                found=asset
                                break
                        
                        if not found:
                            found = Asset("ANIMATION",object,action)
                            animations.append(found)
            
            # Write mesh or collider
            if object.type=='MESH':
                if object.data.name[:2]=="C-":
                    found = None
                    for asset in colliders:
                        if asset.object.data.name==object.data.name:
                            found=asset
                            break
                    
                    if not found:
                        found = Asset("COLLIDER",object)
                        colliders.append(found)
                    instances.append(Instance(found,object))
                    
                else:
                    found=None
                    for asset in meshes:
                        if asset.object.data.name==object.data.name:
                            found=asset
                            break
                               
                    if not found:
                        found = Asset("MESH",object)
                        meshes.append(found)
                        for mat in object.data.materials:
                            found2=False
                            for m in materials:
                                if m.name==mat.name:
                                    found=True
                                    break
                            
                            if not found2:
                                materials.append(mat)
                                
                    instances.append(Instance(found,object))
                        
            # Write Armature    
            elif object.type=='ARMATURE':
                found=None
                for asset in armatures:
                    if asset.object.data.name==object.data.name:
                        found=asset
                        break
                
                if not found:
                    found = Asset("ARMATURE",object)
                    armatures.append(found)
                instances.append(Instance(found,object))

        for asset in animations:
            file = FileWriter(modelPath+asset.action.name+".animation")
            if file.error:
                continue
            
            WriteAnimation(log,file,asset.object,asset.action);
            
            file.close()
            
        for o in materials:
            file = FileWriter(modelPath+o.name+".material")
            if file.error:
                return
            
            WriteMaterial(log,file,o)
            
            file.close()
        
        
        assets = meshes+colliders+armatures
        
        for asset in assets:
            objectCopy=asset.object.copy()
            objectCopy.data=asset.object.data.copy()
            bpy.context.scene.collection.objects.link(objectCopy)
            objectCopy.select_set(True)
            bpy.context.view_layer.objects.active=objectCopy
            
            objectCopy.matrix_world = axisConvert @ objectCopy.matrix_world
            objectCopy.data.transform(axisConvert)
            objectCopy.matrix_world = objectCopy.matrix_world @ axisIConvert
            
            bpy.context.view_layer.update()
            
            asset.name=asset.object.data.name
            
            file = None
            try:
                if asset.type=="MESH":
                    if objectCopy.modifiers.find("Triangulate")==-1:
                        objectCopy.modifiers.new("Triangulate",'TRIANGULATE')
                        #object.modifiers["Triangulate"].quad_method="SHORTEST_DIAGONAL"
                    isBoned = False
                
                    for mod in objectCopy.modifiers:
                        if not mod.type=='ARMATURE':
                            bpy.ops.object.modifier_apply(modifier=mod.name)
                        else:
                            isBoned=True
                    
                    file = FileWriter(modelPath+asset.name+".mesh")
                    if not file.error:
                        WriteMesh(log,file,objectCopy,isBoned,asset.name)
                    
                elif asset.type=="COLLIDER":
                    colliderType = DecideColliderType(objectCopy)
                    
                    if colliderType:
                        for mod in object.modifiers:
                            bpy.ops.object.modifier_apply(modifier=mod.name)
                        
                        # collider name needs to be overwritten
                        asset.name=colliderType+asset.object.data.name[1:]
                        
                        file = FileWriter(modelPath+asset.name+".collider")
                        if not file.error:
                            WriteCollider(log,file,objectCopy,colliderType,asset.name)
                    
                elif asset.type=="ARMATURE":
                    for mod in object.modifiers:
                        bpy.ops.object.modifier_apply(modifier=mod.name)
                    
                    file = FileWriter(modelPath+asset.name+".armature")
                    if not file.error:
                        WriteArmature(log,file,objectCopy,asset.name)
            except:
                print(traceback.format_exc())
                log.print("ERROR","See console!")
            
            if file:
                file.close()
            
            bpy.ops.object.delete()
        
        file = None
        try:
            file = FileWriter(modelPath+root.name+".model")
            if not file.error:
                WriteModel(log,file,instances,animations,root.name)   
        except Exception:
            print(traceback.format_exc())
            log.out("ERROR","See console!")
        
        if file:
            file.close()
        
    # select previously selected objects
    for ob in selectedObjects:
        bpy.ops.object.select_pattern(pattern=ob.name)
    
    if not activeObject==None:
        bpy.context.view_layer.objects.active=activeObject

# The code below is for prefererences
class EGONEProperty(bpy.types.PropertyGroup):
    projectPath : bpy.props.StringProperty(name="Project Path")
    subPath : bpy.props.StringProperty(name="Sub Path")
    
class EGONEPanel(bpy.types.Panel):
    bl_label = "Engone Exporter"
    bl_idname = "VIEW3D_PT_egone"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = "Engone Exporter"
    
    def draw(self,context):
        layout = self.layout
        scene = context.scene
        egone = scene.egone
        
        layout.prop(egone,"projectPath")
        layout.prop(egone,"subPath")
        layout.operator("object.engone_export");

# The code below is for exporting models
class EGONEOperator(bpy.types.Operator):
    bl_label = "Export Models"
    bl_idname = "object.engone_export"
    
    def execute(self, context):
        scene = context.scene
        egone = scene.egone
        
        projectPath = egone.projectPath
        
        log = ExportLog()
        
        # Default path once again
        if projectPath=="":
            log.print("ERROR","Project path is empty!")
        else:
            # No need to worry about ending the with slashes
            projectPath = egone.projectPath if projectPath[-1]=='\\' else projectPath+'\\'
            if len(egone.subPath)>0:
                projectPath = projectPath + (egone.subPath if egone.subPath[-1]=='\\' else egone.subPath+'\\')
            
            ExportAssets(log,projectPath)
        
        global logState
        msg = log.getMessage()
        level = log.getLevel()
        
        self.report({level}, msg)
        return {"FINISHED"}
    
classes = [EGONEProperty,EGONEPanel,EGONEOperator]

def register():
    for cls in classes:
        bpy.utils.register_class(cls)
        
        bpy.types.Scene.egone = bpy.props.PointerProperty(type=EGONEProperty)
 
def unregister():
    for cls in classes:
        bpy.utils.unregister_class(cls)
        
        del bpy.types.Scene.egone
    
if __name__ == "__main__":
    register()