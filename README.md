MINIPROJECT README and TESTING instructions
================================================================================

INDEX:
Ln  14. 1. INTRODUCTION
Ln  32. 2. CODE STRUCTURE
Ln  74. 3. IMPLEMENTATION
Ln 130. 4. TESTING
Ln 295. 5. STOCHASTIC ASPECT
Ln 328. 6. RESULT DISCUSSION
Ln 446. 7. CONCLUSION


1. INTRODUCTION
--------------------------------------------
This is an implementation of a simple Delaunay mesh generator with Python
plotting library. In strict keeping with the README for this project, the
program will only accept input files that are of the type described therein,
and must bound a rectangle. The implementation is such that, were this to be
extended to higher dimensions, or higher order polygons, it should not strain
the developer. More on this in the section on testing. The C++ functions
compile using C++11.

The plotting functionality is moved into a separate Python script that can be
run separately using the instructions provided in this README. As importing the
C++ library for Python plotting was proving to be too painful, I elected to
learn how to use Matplotlib and Numpy in the native Python environment, as
opposed to importing. As it turns out, importing C++ into Python is much easier
than the other way around, although I did not implement this specifically.


2. CODE STRUCTURE
--------------------------------------------
In this project folder, you will find several other folders. Since this
generator ended up creating several different classes, each with associated
implementations, I elected to split the file organization as follows:

FIGURE 1. Code structure:

`+ mp_miniproject/
|
| + docs/
| | | - Documentation files
| + include/
| | | - Class include (.h) files
| + lib/
| | | - External libraries
| + obj/
| | | - Object (.o) files
| + src/
| | | - Implementation source files (.cpp)
| + test/
| | | - Files provided for testing
| | + gold/
| | | | - Gold level testing examples
`
The root mp_miniproject/ folder includes the Makefile (which compiles from the
include/ and src/ folders, and links from the obj/ folder), the mesh-generator
executable, and the plotMesh.py Python executable for plotting the mesh output
from mesh-generator.

The mesh-generator executable takes any number of .txt files as input, and will
product two output files: a <filename>.msh, and <filename>.del.msh. The former
will be the simple, naive mesh generated in Step 1, as a verification that the
original mesh was created as expected. The latter (with the .del prefix) is
the Delaunay-ified version of that same mesh, with optimized minimum interior
angles.

As mentioned in the Introduction, the matplotlib.h external library include
was not used in this project, and as such, the lib/ folder is technically
unnecessary, unless the decision is made in the future to implement the C++
plotter anyway. No plans currently exist for this.

3. IMPLEMENTATION
--------------------------------------------
The mesh-generator.cpp is essentially the "master controller" function that
builds the classes that do all the heavy lifting. Provided the correct number of
input files are given, it will build the "Body" of the input file first. The
Body is in charge of parsing the input file, and ensuring it is sufficient to
make a mesh (i.e. includes the appropriate instructions, more in this in the
testing section).

The Mesh class takes a Body as an input, and turns it into a "mesh". The only
thing a Mesh does, is generate the node layout that defines the "mesh"
structure. This is considered separate from the next layer down, the
Triangulation. The Triangulation takes the Mesh, and builds elements from the
nodes. The Triangulation does no internal checking, it just takes nodes, and
connects them with Edges, and builds Elements.

The reasoning for considering the Mesh and Triangulation to be separate is that
they perform fundamentally different tasks. Even though there are members in
the Mesh class that just call the same member in the Triangulation function,
that is just for accessibility from the front-end. The Triangulation is an
operation that is performed on a level "below" the nodes themselves. Generally,
in computational mechanics, we consider the Node to the more important measure
of mesh quality. A Node defines a "degree of freedom" from a mechanics
perspective, whereas an Element is used on the quadrature and integration
level.

With the implementation like this, it is much easier to implement a method that
does the quadrature over the Triangulation in the Mesh class, rather than having
everything live in the same class. This would make the Mesh class incredibly
bulky, and essentially do a whole bunch of things it is not intended to do. So
as it stands right now, the Mesh might look very light, but it will feel more
fleshed out once other computational mechanics methods are implemented.

At the very bottom of the class structure are the Nodes, Edges and Elements.
Nodes are really the most basic, stand-alone objects here. While I elected to
make Nodes "know" about the Edges that connect them, there is no real reason
for this other than convenience, and other implementations can work just as
well. Nodes are defined simply by their x,y-coordinates, and have an associated
ID that is really only used in printing (and can be useful in any stiffness-
matrix building methods).

Edges are defined by the Nodes that connect them. Every Edge is really two
Euclidean vectors, which make Edges directional. The reason for the
directionality, is that it allows for finding which other Node a given Node is
closer to. It also makes certain linear algebra methods that I chose to
implement for finding coordinates much more natural and straight forward.
The only downside is that it makes searching much higher order, but since
everything is done with pointers or references, the speed cost is not huge,
especially for smaller meshes.

Elements are also just comprised of Nodes, although the Element class also
keeps track of which Edges bound it, for the simple reason that those will be
used in a variety of methods included in Elements, so it reduces the number of
Edges to search through up front. Elements and Nodes then make up the
Triangulation.

4. TESTING
--------------------------------------------
In the folder test/ you will find several input*.txt files. Each of these
contains instructions for making a mesh to illustrate the functionality of this
program. (do > ./run_all_tests.sh to run all tests in bulk)

You may want to begin by running:
`rm test/*.msh test/*.log`
or alternatively:
`make clean`
This will remove all the mesh files and error log files already created in the
test/ folder, to allow you to start with a clean slate.

Below is a table that outlines the purpose of each of the input*.txt files in
the test/ folder. The assumption here, is that given an input file, the user is
able to generate the expected output mesh by hand, that means generating the
nodes, and elements from the instructions provided. This will be required for
confirming the results are in fact correct. To avoid any ambiguity, section 6
below walks you through the exact process implemented here, so we can confirm
the output matches expectations.

If there is an output to be generated from the given input file, there will be
an associated "expected output" image file in the test/gold/ folder, which will
be labeled in*.png for the simple mesh, that would be generated in Step 1 of the
README, and de*.png for the Delaunay meshing in Step 2.

TABLE 1. Testing input files

--+-------------+--------------------+------------------------------------------
# | Filename    | Expected Output    | Reason for including
==+=============+====================+==========================================
1 | input1.txt  | test/gold/in1.png  | Displays basic functionality as expected
  |             | test/gold/de1.png  | from the README.
--+-------------+--------------------+------------------------------------------
2 | input2.txt  | test/gold/in2.png  | Shows the same mesh under different input
  |             | test/gold/de2.png  | parameters.
--+-------------+--------------------+------------------------------------------
3 | input3.txt  | test/gold/in3.png  | Same as input2, but in the y-direction
  |             | test/gold/de3.png  |
--+-------------+--------------------+------------------------------------------
4 | input4.txt  | Error -- input file contains too many lines (is actually an
  |             | output file from a previous test). Invalid, does not generate
  |             | a mesh.
--+-------------+---------------------------------------------------------------
5 | input5.txt  | Error -- input file does not contain enough lines (omitted one
  |             | of the x/y element size parameters, will perform same if any
  |             | other line were omitted. Just a simple line# check).
--+-------------+---------------------------------------------------------------
6 | input6.txt  | Error -- input file contains an invalid character (anything
  |             | that is not a number), causing the stod() method to throw an
  |             | exception. Throws all the way up and does not generate mesh.
--+-------------+---------------------------------------------------------------
7 | input7.txt  | Error -- the x/y element size parameters must be a single
  |             | double number, and cannot include any trailing characters.
--+-------------+--------------------+------------------------------------------
8 | input8.txt  | test/gold/in8.png  | The node numbering must be integer, but
  |             | test/gold/de8.png  | may include a decimal point with trailing
  |             |                    | zeros. Otherwise same as #1. Must start
  |             |                    | at 1, cannot start at 0.
--+-------------+--------------------+------------------------------------------
9 | input9.txt  | Error -- node numbering here is not in a valid format. I.e.
  |             | the ID prescribed in the input file is an invalid decimal
  |             | (when truncated at decimal point is no longer the same number)
--+-------------+---------------------------------------------------------------
10| input10.txt | Error -- node definition does not take correct form, there is
  |             | an extra number appended to the end of the node.
--+-------------+---------------------------------------------------------------
11| input11.txt | Error -- node definition does not take correct form, one of
  |             | the required parameters is missing (omitted the y-coord).
--+-------------+--------------------+------------------------------------------
12| input12.txt | test/gold/in12.png | Shows behavior if element size is too
  |             | test/gold/de12.png | large in BOTH directions. Just draw a
  |             |                    | diagonal from bottom left to top right.
--+-------------+--------------------+------------------------------------------
13| input13.txt | Error -- the input file describes a degenerate shape (one
  |             | with zero height or width). Do not generate line elements.
--+-------------+---------------------------------------------------------------
14| input14.txt | Error -- node ordering not correct. In the input file, nodes
  |             | must be ordered by y first, then x, so that they can be
  |             | ordered like bottom-left => bottom-right => top-left => top-
  |             | right. The order of appearance in the file does not matter, as
  |             | long as they are numbered in accordance with this. So an input
  |             | like "3 0.0 2.5/ 2 3.0 0.0/ 4 3.0 2.5/ 1 0.0 0.0" is fine,
  |             | because the numbering allows for them to be connected in the
  |             | correct way, but "1 3.0 0.0/ 2 0.0 0.0/ ..." does not.
--+-------------+--------------------+------------------------------------------
15| input15.txt | test/gold/in15.png | The correct case described in #14
  |             | test/gold/de15.png |
--+-------------+--------------------+------------------------------------------
16| input16.txt | Error -- input nodes do not describe a rectangle. Cannot be a
  |             | trapezoid, parallelogram or anything else. Must be strictly a
  |             | rectangle, in accordance with the README.
--+-------------+--------------------+------------------------------------------
17| input17.txt | test/gold/in17.png | The rectangle does not have to be
  |             | test/gold/de17.png | contained in the first quadrant on the
  |             |                    | x,y-plane, as long as other conditions
  |             |                    | are satisfied.
--+-------------+--------------------+------------------------------------------
18| input18.txt | test/gold/in18.png | Illustrates performance when element size
  |             | test/gold/de18.png | is very small, and lots of elements have
  |             |                    | to be created. A finer version of #17.
--+-------------+--------------------+------------------------------------------

Since the majority of the validating is done by the Body class, and some
secondary validation is done by the Mesh class, this should be sufficient to
ensure correct functionality of the program. As long as the points that are
given to the Triangulation class coming from the Mesh class are valid, then the
Triangulation class produces the output that one would achieve if creating the
mesh by hand on paper.

Of course, to generate the outputs above for yourself, you can run all the input
files in the test/ folder by simply running:
`./mesh-generator test/*`
As long as you have removed all the pre-generated mesh files from this folder,
you should be getting outputs in the order of the table above. If not, you will
get several errors pertaining to failure to read the .msh files. This is
obviously expected. Alternatively, you can run all tests with the included
bash script:
`./run_all_tests`

The .msh files are really just comma separated values (csv), which are pretty
universally supported by whatever plotting software you choose to use (be it
MATLAB, Python, Octave, etc.). The implementation chosen here is Python for
it being terminal friendly. To view the meshes generated by mesh-generator,
simply run the following in terminal:
`python ./plotMesh.py test/*.msh`
This will open a separate window with plots of all the meshes that succeeded. Of
course, you can verify that the outputs you get are in line with the expected
outputs in the test/gold/ folder (you'll have to pull the repo down to your
machine to support opening the .png files).

The plotMesh.py function is written in Python 2, and requires the Numpy, csv,
sys and matplotlib packages to be accessible to the Python interpreter. If
Python 2 is not the default Python version on your machine, you can install it
from the Python website or from terminal. Plenty of instructions for that
online. To run in Python 2 if it is not default, you can change the command to:
`python2 ./plotMesh.py test/*.msh`

There are a few aspects that should be discussed about the results you will
find in the test/gold/ folder. While those have been generated from the given
input files, the sub-folder test/gold/High_res_examples/ includes some runs
where I pushed test case #18 above to the limit. In general, because of the
implementation, and how the simple mesh is drawn, if the element size is too
small, the simple mesher will run into floating point errors, as it needs to
check which element a given node is in, although it can be arbitrarily close
to one of the edges. Since I am using dot-product like construction to figure
out where the node is, this can give potentially irrational expressions which
cannot be fully represented by a double. The mesher will complain that it has
generated a line element (essentially 3 points on a line), whenever this has
occurred.

The examples in the High_res_examples/ folder are some cases where a mesh
could be generated, but where the Delaunay process did not fully converge to
the structured shapes we see in the test/gold/ folder. The reason for this,
and it is obvious looking at the pictures, is that the Delaunay process as
described in the README is somewhat sensitive to initial conditions. The
regions that clearly vary in structure, are regions where the Delaunay mesher
simply cannot swap edges without generating line elements, or making the
internal angles smaller. Phenomena like this tend to show up more and more as
the elements become smaller, as there will be more lines that tend to converge
in one corner of the body. This can be mitigated by randomizing the nodes,
which will remove the regularity of the mesh points, allowing this structure
to be broken.


5. STOCHASTIC ASPECT
--------------------------------------------
Step 3 of the README requires there to be some randomization of the Node
placement, which obviously introduces some stochastic aspect to the meshing
process. For this reason, the randomized node placement is not included in the
testing verification of the previous step, as there will not be a 1:1
comparison that can be done to verify the outputs are the same (if there were
two identical stochastic meshes generated in subsequent calls to the function,
you should go and get lottery tickets immediately).

However, given some input file, there is some qualitative checking that can be
done to see that different outputs agree. So in the test/gold/ folder, you will
furthermore find files labeled rd*.png, which contain Delaunay meshes that have
been generated after the node positions were "randomized". Clearly, whatever
mesh you will generate from the input files will be different from the meshes
shown in here, but the general idea should be the same.

Of course, the randomization is not without bounds. The way the randomization is
done, is that the nodes are allowed to translate within a certain circumference
that intersects the bounding rectangle. So a node on the outside edge can only
move along that line, nodes in the interior can move in any direction they want,
as long as it does not land them outside the bounds of the rectangle.

The rule for moving nodes is that they can only move a distance that is 10% of
the x-size or y-size of the element. That means if the first two
lines of the input file are "0.2/ 0.8", then the nodes can move at most a
distance of 0.04 in the x-direction and at most 0.16 in the y-direction. The
nodes will be moved in the Mesh class, and then passed to the Triangulation
class, which will continue to function the same as in the previous steps. In
general, assume the Triangulation methods are "stupid", and the Mesh and Body
classes have to babysit it to make sure it does not screw up.


6. RESULT DISCUSSION
--------------------------------------------
While the errors back in the Testing section are descriptive, how would we know
whether the outputs that do not result in errors are correct? The general
procedure is the same for any number of nodes (although it may take you a
rather long time to finish drawing the mesh by hand).

In this section, I will walk you through the implemented procedure, and what
output is expected (using my interpretation of the README, to show you how I
reached my conclusions). Let's start with the input file given in the README:

1.0
1.25 (corrected from 1.35, since I suppose this is a typo)
1 0.0 0.0
2 3.0 0.0
3 0.0 2.5
4 3.0 2.5

This defines a rectangle from (0, 0) to (3, 2.5), with a maximum element x-size
of 1 and max y-size of 1.25. Since the width of the rectangle is 3, this divides
perfectly in 3 units. So there will be 3 elements on the horizontal bounds of
the rectangle. Similarly for the vertical bounds, but 1.25 divides 2.5 only
twice. Now, if these sizes were different, say, an x-size of 2, then the maximum
element size that would divide the bottom and top edges evenly would be 1.5.
For now, we have the following:

FIGURE 2. Body node placement

  3                    4
  x                    x



  x                    x
  1                    2

Note, they are not connected with edges yet. Edges don't yet exist, so I have
not drawn them.

The next step, is to generate the intermediate points. Per the instructions
above, we divide the edges evenly with the maximum side length permitted by the
size that were input, sorting them by x first, then y. Per the previous
instructions, the mesh will look like this:

FIGURE 3. Meshed node placement

  3      8      11     4
  x      x      x      x

5 x    7 x   10 x      x 12

  x      x      x      x
  1      6      9      2

Of course, if the input file were different, this would look different. For
example, if the x-size were 2, there would only be 9 nodes, with nodes 6,7,8
located in the center, and node 9 being where node 12 is in the Figure above.

This is the setup that can be randomized if desired, causing nodes 5,6,8,9,
11,12 to move along the outside edges they are located on (even though there)
are no Edge objects, yet), and nodes 7,10 can move in any x,y-direction.

The Triangulation is then passed the nodes generated in the last Figure,
will generate the outermost edges (always defined by the first 4 nodes, in
the order they are shown above, even if they were not defined in that order
as they appear in the input file, they must be numbered in that order), and
add the first node (if it exists). If there is no first node, it will simply
draw the 1-4 diagonal, and be done. If there is a first node after the edges,
it connects it to all the edges as follows:

FIGURE 4. Triangulation first step

  3                    4
  x--------------------x
  |   ________________/|
5 x <                  |
  |  `````````````````\|
  x--------------------x
  1                    2

Of course, there is only so much I can show you with ASCII characters in this
document. In the following steps, it will add the nodes in order, finding
which element they are "inside" of, or on the edge of, and split that element
at that node (in two if on edge, in three if in interior). So when Node 6 is
added, the figure will look like:

FIGURE 5. Triangulation second step

  3                    4
  x--------------------x
  |   _______________ /|
5 x <___               |
  |   \ ``````````````\|
  x-----x--------------x
  1     6              2

Here, 6 has split the edge 1-2, and connected to node 5, generating the new
set of elements 1-5-6 and 2-5-6. When 7 is added, it will split the element
2-4-5, etc.

The final result of this procedure is shown in test/gold/in1.png.

I will not illustrate the Delaunay-fication of this mesh, as it tends to be very
different from one problem to another, but the general idea is that we compute
all the interior angles of two adjacent elements, and swap the diagonal if that
achieves a larger minimum interior angle. We basically loop over all the
elements, finding their adjacent ones, and swapping if it yields a better
result. Larger interior angles means that the Jacobian that is used in changing
coordinates in the Finite Element Method is better conditioned, and the elements
are not degenerate resulting in errors in matrix inversion.

The way this is implemented here, is by setting a flag that indicates diagonals
were swapped this round going over all the elements. If no more diagonals are
swapped, the flag is removed, and the while loop stops. This also sets a flag
in the Triangulation class that the mesh has been Delaunay-ified, which produces
the unique name to the output file.


7. CONCLUSION
--------------------------------------------
This document should serve as a sufficient README for this project, and
hopefully outlines the functionality to a point where usage will take minimal
additional input from the user. As mentioned in a previous section, the way
the functionality is laid out allows for more straightforward implementation of
more sophisticated methods intrinsic to computational mechanics, like building
stiffness matrices, mass matrices, setting boundary conditions, building more
complex shapes, etc.

While the technology developed herein is by no means novel, it does serve as a
good learning experience for those getting into computational mechanics, or
whose career/research is somehow adjacent to this topic. Finite Element lectures
often skip over a lot of this, or will have the student generate meshes by hand
(which can be tedious, to say the least). This project is a fantastic
introduction for students to learn how to automate this kind of process (or how
their favorite commercial mesher goes about doing this).
