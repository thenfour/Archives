file$ = "box.3d2"

sheight = 10
sinspeed = 50
vxmax = 250: vxmin = -250
vymax = 250: vymin = -250
vzmax = 300: vzmin = -35
screenmode = 7
cameraz = 0
xspeed = 0
yspeed = 0
zspeed = 0

d = 1024

xra = 5
yra = 2
zra = 4

pi = 3.141592654#
vs = 1: ws = 0
CLS

'make an array for faster calculation of sine and cosine
DIM COSine(720), SINe(720)
FOR i = 0 TO 720
COSine(i) = COS((i - 360) * pi / 180)
SINe(i) = SIN((i - 360) * pi / 180)
NEXT i


'read in the data into variables.  vx stands for Virtual X coordinate
OPEN file$ FOR INPUT AS 1

INPUT #1, sides
DIM vx(3, sides), vy(3, sides), vz(3, sides)
DIM vx2(3, sides), vy2(3, sides), vz2(3, sides)
DIM ox1(3, sides), ox2(3, sides), oy1(3, sides), oy2(3, sides)
DIM dx(3, sides), dy(3, sides)
DIM ofn(sides), avgz(sides), avgx(sides), avgy(sides)
DIM sorted(sides)
DIM refarray(3, sides, 1), ref(3, sides)

'when the refarray for a particular point is non-zero, then use
'refarrya(i,sides,0) for the reference 'i', and
'refarray(i,sides,1) for the reference 'sides'.


'listing of variables used in here:
'
'xra,yra,zra = angle to rotate each loop (speed)
'vx,vy,vz = the virtual coords that we dont mess with.
'ox1,oy1 = old screen coords for first point of each line
'ox2,oy2 = old point two of each line
'dx1,dy1 = screen coords for first point of line
'dx2,dy2 = screen coords for 2nd point of the line
'vx2,vy2,vz2 = the virtual coords that we change and manipulate
'ofn = whether the face is visible or not
'avg = the average Z coord for each face (used in sorting)
'sorted = an array that gives you the faces numbers, back to front
'refarray = the reference point for a point that is used more than once.
'           (so we dont have to manipulate the same point more than once)

FOR i = 1 TO sides
FOR x = 1 TO 3
INPUT #1, vx(x, i), vy(x, i), vz(x, i)
NEXT
NEXT

CLOSE #1

'find the max and min x and y's so that we know when to bounce it.
minx1 = 0: maxx1 = 0
miny1 = 0: maxy1 = 0
minx2 = 0: maxx2 = 0
miny2 = 0: maxy2 = 0
minz1 = 0: minz2 = 0
minz2 = 0: maxz2 = 0

FOR k = 1 TO sides
FOR i = 1 TO 3
        IF vx(i, k) < vx(minx1, minx2) THEN minx1 = i: minx2 = k
        IF vx(i, k) > vx(maxx1, maxx2) THEN maxx1 = i: maxx2 = k
        IF vy(i, k) < vy(miny1, miny2) THEN miny1 = i: miny2 = k
        IF vy(i, k) > vy(maxy1, maxy2) THEN maxy1 = i: maxy2 = k
        IF vz(i, k) < vz(minz1, minz2) THEN minz1 = i: minz2 = k
        IF vz(i, k) > vz(maxz1, maxz2) THEN maxz1 = i: maxz2 = k
NEXT
NEXT


'make that array.
FOR aside = 1 TO sides
FOR anum = 1 TO 3
        FOR bside = 1 TO aside
        FOR bnum = 1 TO 3
                IF bnum = anum AND bside = aside THEN bnum = 3: bside = aside: GOTO nbnum
                IF vx(anum, aside) = vx(bnum, bside) AND vy(anum, aside) = vy(bnum, bside) AND vz(anum, aside) = vz(bnum, bside) THEN
                        refarray(anum, aside, 0) = bside
                        refarray(anum, aside, 1) = bnum
                        ref(anum, aside) = 1
                        bside = aside
                        bnum = 3
                END IF
       
nbnum:
        NEXT bnum
        NEXT bside
NEXT anum
NEXT aside


'************************************************
'this is where the main loop begins
lupe:
        'swap the working screen and visible screen
        WAIT &H3DA, 8
       
        SWAP vs, ws
        SCREEN screenmode, , ws, vs
        WINDOW (-320, -200)-(320, 200)
rot = (rot + 1) MOD 360
        'shift all points in the direction we are going
        FOR k = 1 TO sides
        FOR i = 1 TO 3
                xshift = xshift + xspeed
                yshift = yshift + yspeed
                zshift = zshift + zspeed
        NEXT
        NEXT
               
                IF zshift < vzmin THEN zspeed = -zspeed
                IF zshift > vzmax THEN zspeed = -zspeed
                IF yshift < vymin THEN yspeed = -yspeed
                IF yshift > vymax THEN yspeed = -yspeed
                IF xshift < vxmin THEN xspeed = -xspeed
                IF xshift > vxmax THEN xspeed = -xspeed
               
        'this is where we rotate the points.


sfactor = sheight + (sheight * SINe((sinspeed * rot) MOD 360))

FOR k = 1 TO sides
FOR i = 1 TO 3

        IF ref(i, k) <> 0 THEN
                        vx2(i, k) = vx2(refarray(i, k, 1), refarray(i, k, 0))
                        vy2(i, k) = vy2(refarray(i, k, 1), refarray(i, k, 0))
                        vz2(i, k) = vz2(refarray(i, k, 1), refarray(i, k, 0))
                        ELSE
               
                vx2(i, k) = vx(i, k)
                vy2(i, k) = vy(i, k)
                vz2(i, k) = vz(i, k)
               

                IF i = 1 THEN
                IF k > 0 AND k < 5 THEN vz2(i, k) = vz2(i, k) - sfactor
                IF k > 4 AND k < 9 THEN vx2(i, k) = vx2(i, k) + sfactor
                IF k > 8 AND k < 13 THEN vz2(i, k) = vz2(i, k) + sfactor: '-
                IF k > 12 AND k < 17 THEN vx2(i, k) = vx2(i, k) - sfactor: '-
                IF k > 16 AND k < 21 THEN vy2(i, k) = vy2(i, k) - sfactor
                IF k > 20 AND k < 25 THEN vy2(i, k) = vy2(i, k) + sfactor: '-

                END IF

                IF xra <> 0 OR cameraxr <> 0 THEN
                        a = (cameraxr + (rot * xra)) MOD 360
                        x = vx2(i, k): y = vz2(i, k):
                        GOSUB rotate
                        vx2(i, k) = x: vz2(i, k) = y
                END IF
                IF yra <> 0 OR camerayr <> 0 THEN
                        a = (camerayr + (rot * yra)) MOD 360
                        x = vz2(i, k): y = vy2(i, k):
                        GOSUB rotate
                        vz2(i, k) = x: vy2(i, k) = y
                END IF
                IF zra <> 0 OR camerazr <> 0 THEN
                        a = (camerazr + (rot * zra)) MOD 360
                        x = vx2(i, k): y = vy2(i, k):
                        GOSUB rotate
                        vx2(i, k) = x: vy2(i, k) = y
                END IF
                        vz2(i, k) = vz2(i, k) + zshift + cameraz
                        vx2(i, k) = vx2(i, k) + xshift + camerax
                        vy2(i, k) = vy2(i, k) + yshift + cameray
               
                END IF
              
      

        NEXT i, k
nozra:
       
       
        'this just goes thru and figures out the screen coords of each point

        FOR k = 1 TO sides
        FOR i = 1 TO 3
                dx(i, k) = (d * (vx2(i, k))) / (vz2(i, k) + d)
                dy(i, k) = (d * (vy2(i, k))) / (vz2(i, k) + d)
        NEXT i
        NEXT k
CLS
    'draw the sides
        FOR k = 1 TO sides
                'ref = sorted(k)
                ref = k
                x1 = dx(1, ref): x2 = dx(2, ref): x3 = dx(3, ref)
                y1 = dy(1, ref): y2 = dy(2, ref): y3 = dy(3, ref)
                c = (x2 - x1) * (y3 - y2) - (x3 - x2) * (y2 - y1)
        IF c > 0 THEN
                ofn(ref) = 1
                averagex = 0: averagey = 0
                colour = 1 + (ref MOD 16)
                FOR i = 1 TO 3
                       IF i = 3 THEN
                       LINE (dx(3, ref), dy(3, ref))-(dx(1, ref), dy(1, ref)), colour
                       ELSE
                       LINE (dx(i, ref), dy(i, ref))-(dx(i + 1, ref), dy(i + 1, ref)), colour
                       END IF
                averagex = averagex + dx(i, ref)
                averagey = averagey + dy(i, ref)

                NEXT i
                avpaintx1 = averagex / 3
                avpainty1 = averagey / 3

                'gosub paintpolygon
                PAINT (avpaintx1, avpainty1), colour, colour
                PAINT (-319, -199), 0
        ELSE
                IF ofn(ref) = 1 THEN
                        ofn(ref) = 0
                END IF
        END IF
NEXT k
k$ = INKEY$
IF k$ <> "" THEN END

     GOTO lupe:
rotate:
x2 = x
x = COSine(a) * x - SINe(a) * y
y = SINe(a) * x2 + COSine(a) * y
RETURN

paintpolygon:

