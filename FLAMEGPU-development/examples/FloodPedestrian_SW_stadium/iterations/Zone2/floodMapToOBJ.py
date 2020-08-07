import re
import sys

verts = []

def opXYZ(op, vertsList):
  opx = op([float(x) for (x, y, z) in vertsList])
  opy = op([float(y) for (x, y, z) in vertsList])
  opz = op([float(z) for (x, y, z) in vertsList])
  return (opx, opy, opz)

maxXYZ = lambda vertsList : opXYZ(max, vertsList)
minXYZ = lambda vertsList : opXYZ(min, vertsList)

inFileName = sys.argv[1]

# Open XML file for reading
with open(inFileName) as f:
  lineCounter = 0
  inXAgent = False
  inFloodCell = False
  x = 0
  y = 0
  z = 0
 
  for line in f:
    # Are we in an xagent element
    if line.strip() == "<xagent>":
      inXAgent = True
    if line.strip() == "</xagent>":
      inXAgent = False
      inFloodCell = False
      lineCounter = 0

    # If we are, is the xagent a Flood Cell
    if inXAgent:
      if line.strip() == "<name>FloodCell</name>":
        inFloodCell = True
     
    # If we are in a flood cell, record the x,y,z vals from the following lines
    if inFloodCell:
      if lineCounter == 2:
        # Read x val
        x = re.sub('<[^<]+?>', '', line.strip())
      if lineCounter == 3:
        # Read y val
        y = re.sub('<[^<]+?>', '', line.strip())
      if lineCounter == 4:
        # Read z val
        z = re.sub('<[^<]+?>', '', line.strip())
        # Add data to verts list
        verts.append((x, y, z))
      lineCounter = lineCounter + 1

minVerts = minXYZ(verts)
maxVerts = maxXYZ(verts)
maxx = round(maxVerts[0])
maxy = round(maxVerts[1]) 
# Remove any offset
zeroedVerts = [(float(x) - minVerts[0], float(y) - minVerts[1], float(z) - minVerts[2]) for (x, y, z) in verts]
# Scale to 0..1 range
normalisedVerts = [(float(x) / maxVerts[0], float(y) / maxVerts[1], float(z) / maxVerts[2]) for (x, y, z) in zeroedVerts]
# Scale to -1 to 1 range
scaledVerts = [(x * 2.0 - 1, y * 2.0 - 1, z * 2.0 - 1) for (x, y, z) in normalisedVerts]

def coordsToVertIndex(x, y, xmax):
  return x*xmax + y

with open("map.obj", 'w') as f:
  # Write vertices to file
  for (x, y, z) in scaledVerts:
    f.write('v ' + str(x) + ' ' + str(y) + ' ' + str(z) + '\n')
  
  ctvi = lambda x, y : str(1 + coordsToVertIndex(x, y, maxx + 1))
  
# Generate triangluar faces
  for x in range(0, maxx):
    for y in range(0, maxy):
      f.write('f ' + ctvi(x, y) + ' ' + ctvi(x + 1, y + 1) + ' ' + ctvi(x + 1, y) + '\n')
      f.write('f ' + ctvi(x, y) + ' ' + ctvi(x, y + 1) + ' ' + ctvi(x + 1, y + 1) + '\n')
  
with open("water_map.obj", 'w') as f:
  # Write vertices to file
  for (x, y, z) in scaledVerts:
    f.write('v ' + str(x) + ' ' + str(y) + ' ' + str(0.0) + '\n')
  
  ctvi = lambda x, y : str(1 + coordsToVertIndex(x, y, maxx + 1))
  
# Generate triangluar faces
  for x in range(0, maxx):
    for y in range(0, maxy):
      f.write('f ' + ctvi(x, y) + ' ' + ctvi(x + 1, y + 1) + ' ' + ctvi(x + 1, y) + '\n')
      f.write('f ' + ctvi(x, y) + ' ' + ctvi(x, y + 1) + ' ' + ctvi(x + 1, y + 1) + '\n')
  
