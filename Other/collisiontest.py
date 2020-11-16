def sub(v0,v1):
    return [v0[0]-v1[0],v0[1]-v1[1],v0[2]-v1[2]]

def add(v0,v1):
    return [v0[0]+v1[0],v0[1]+v1[1],v0[2]+v1[2]]

def mul(v0,f):
    return [v0[0]*f,v0[1]*f,v0[2]*f]

def length(v):
    return pow(pow(v[0],2)+pow(v[1],2)+pow(v[2],2),1/2)

def normalize(v):
    l=length(v)
    return (v[0]/l,v[1]/l,v[2]/l)


def dot(v0, v1):
    return v0[0]*v1[0]+v0[1]*v1[1]+v0[2]*v1[2]

def cross(v0,v1):
    vo = [0,0,0]
    vo[0]=v0[1] * v1[2] - v1[1] * v0[2]
    vo[1]=v0[2] * v1[0] - v1[2] * v0[0]
    vo[2]=v0[0] * v1[1] - v1[0] * v0[1]
    return vo

def LinePlane(plane_p, plane_n, l0, l1):
    plane_d = dot(plane_n, plane_p);
    ad = dot(l0, plane_n);
    bd = dot(l1,plane_n);
    if(bd-ad==0):
        t=999999
    else:
        t = (plane_d - ad) / (bd - ad);
    
    lineStartToEnd = sub(l1,l0);
    lineToIntersect = mul(lineStartToEnd,t);
    finish = add(l0,lineToIntersect);
    
    return finish;

def PointTri(A, B, C, P):
    s1 = C[1] - A[1]
    s2 = C[0] - A[0]
    s3 = B[1] - A[1]
    s4 = P[1] - A[1]

    w1 = (A[0] * s1 + s4 * s2 - P[0] * s1) / (s3 * s2 - (B[0] - A[0]) * s1)
    w2 = (s4 - w1 * s3) / s1
    print(str(w1)+" "+str(w2))
    return w1 >= 0 and w2 >= 0 and (w1 + w2) <= 1

up=(-0.841471,0,-0.540302)
v0=(-0.170153,0,-0.134136)
out=(v0[0]*(1-abs(up[0])),v0[1]*(1-abs(up[1])),v0[2]*(1-abs(up[2])))
print(up)
print(out)

##p0=(1,1,1)
##p1=(2,1,1)
##p2=(1,2,1.5)
##r0=(1.5,2,1.25)
##r1=(1.5,1,1.5)
##pn=normalize(cross(sub(p1,p0),sub(p2,p0)))
##
##I=LinePlane(p0,pn,r0,r1)
##
##a=[1,1,1]
##b=[3,1,1]
##c=[1,1,1.5]
##i=LinePlane(a,normalize(cross(sub(b,a),sub(c,a))),(1.5,0.5,1.2),(1.4,1.5,1.2))
##PointTri(a,b,c,i)


