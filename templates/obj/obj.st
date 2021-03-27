obj(vertex_count, face_count, material_count, vertices, texture_vertices, normals, indices) ::= <<
#	                Vertices: <vertex_count>
#	                  Points: 0
#	                   Lines: 0
#	                   Faces: <face_count>
#	               Materials: <material_count>

o 1

# Vertex list

<vertices:{ vertex |v <vertex.value0; format="%,.4f"> <vertex.value1; format="%,.4f"> <vertex.value2; format="%,.4f"> <vertex.value3> <vertex.value4> <vertex.value5><\n>}><texture_vertices:{ texture_vertex |vt <texture_vertex.value0; format="%,.4f"> <texture_vertex.value1; format="%,.4f"> <texture_vertex.value2; format="%,.4f"> <\n>}><normals:{ normal |vn <normal.value0; format="%,.4f"> <normal.value1; format="%,.4f"> <normal.value2; format="%,.4f"> <\n>}>
# Point/Line/Face list

usemtl Default
s off
<indices:{ index |f <index.value0>/<index.value0>/<index.value1> <index.value2>/<index.value2>/<index.value3> <index.value6>/<index.value6>/<index.value7> <index.value4>/<index.value4>/<index.value5><\n>}>
# End of file
>>