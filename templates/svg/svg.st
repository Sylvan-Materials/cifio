svg(height, width, offset_x, offset_y, circles, has_arcs, arcs, has_text, text_list) ::= <<
\<svg xmlns="http://www.w3.org/2000/svg" height="<height; format="%,.4f">" width="<width; format="%,.4f">">
  \<style>
    .small { font: normal 4px sans-serif; }
    .heavy { font: bold 10px sans-serif; }
  \</style>
  \<defs>
    \<marker id="arrowhead" markerWidth="5" markerHeight="3.5" 
    refX="0" refY="3.5" orient="auto">
      \<polygon points="0 0, 5 1.75, 0 3.5" fill="darkred" stroke="black"/>
    \</marker>
  \</defs>
   \<rect x="0" y="0" width="100%" height="100%" fill="white" />
   \<g transform="translate(<offset_x; format="%,.4f"> <offset_y; format="%,.4f">), scale(4,4)">
<circles:{ circle |    \<circle cx="<circle.value0; format="%,.4f">" cy="<circle.value1; format="%,.4f">" r="<circle.value2; format="%,.4f">" stroke="<circle.value3>" stroke-width="0.2" fill="none"/><\n>}>
<if(has_arcs)>  \<g stroke="red"  stroke-width="0.2" fill="none"><\n><arcs:{arc |    \<path d = "M <arc.value0; format="%,.4f"> <arc.value1; format="%,.4f"> L <arc.value3; format="%,.4f"> <arc.value4; format="%,.4f"> A <arc.value2; format="%,.4f"> <arc.value2; format="%,.4f"> 0 <arc.value7> <arc.value8> <arc.value5; format="%,.4f"> <arc.value6; format="%,.4f"> " stroke="<arc.value9>"/><\n>}>  \</g><\n><else>print("no arcs");<endif>
<if(has_text)><text_list:{ text | \<text x="<text.value0; format="%,.4f">" y="<text.value1; format="%,.4f">" class="small"><text.value2; format="%,.2f">\</text><\n>}><else>print("no text");<endif>
  \</g>
\</svg>
>>
