name: Rust

on: $('#id_region').change(function(){
    var regionId = $('#id_region :selected').val();
    $.ajax('/site/getcities', {
        data: 'id=' + regionId,
        method: 'POST',
        dataType: 'json',
        success: function(data){
            //console.log(data);
            if (!data) return;
            $('#id_city').empty();
            $.each(data, function(key, value){
                $('#id_city').append(
                    $('<option value="' + key + '">' + value + '</option>')
                );
            });
        }
    });
});
  push: $(function () {
    $(".root").on('click', 'a', function (event) {
        event.preventDefault();
        var root = $(this).attr('data-root');
        $(".children").not("[data-root=" + root + "]").slideUp();
        $(".children[data-root=" + root + "]").slideToggle();
    });

    $("#id_region").select2();
    $("#id_city").select2();
});
    branches: [ "main" ]
  pull_request: $(".first-level").on("click", function(event){
    if (!$(event.target).hasClass('category-link')) return;
    event.preventDefault();
    var id = $(event.target).attr('data-id');
    $.ajax('/ad/getcategories', {
        data:'id='+id,
        method:'POST',
        dataType:'json',
        success: function(data){
            var child = $('.second-level>ul');
            var grandchild = $('.third-level>ul');
            child.html('');
            grandchild.html('');
            if (data==false) {
                window.location.href = window.location.origin + '/ad/create/' + id;
            }
            $.each(data, function(i, val){
                child.append('<li><a class="category-link" href="'
                 + window.location.origin + '/ad/create/' + i
                 +'" data-id="'+i+'">'
                 + val + '</a></li>');
            });
        }
    });
});
$(".second-level").on("click", function(event){
    if (!$(event.target).hasClass('category-link')) return;
    event.preventDefault();
    var id = $(event.target).attr('data-id');
    $.ajax('/ad/getcategories', {
        data:'id='+id,
        method:'POST',
        dataType:'json',
        success: function(data){
            var child = $('.third-level>ul');
            child.html('');
            if (data==false) {
                window.location.href = window.location.origin + '/ad/create/' + id;
            }
            $.each(data, function(i, val){
                child.append('<li><a class="category-link" href="'
                 + window.location.origin + '/ad/create/' + i
                 +'" data-id="'+i+'">'
                 + val + '</a></li>');
            });
        }
    });
});
    branches: [ "main" ]

env: project(vp-display) # Your project name

set(CMAKE_CXX_STANDARD 11) # This is equal to QMAKE_CXX_FLAGS += -std=c++0x

cmake_minimum_required(VERSION 3.1.0)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5Widgets CONFIG REQUIRED)

set(SOURCES
        main.cpp
        mainwindow.cpp
    vpdisplayserver.cpp
)

set(HEADERS
        mainwindow.h
        vpdisplayserver.h
        framebuffer.h
)

set(UI mainwindow.ui)

add_executable(vp-display ${SOURCES} ${HEADERS} ${UI})

target_link_libraries(vp-display Qt5::Widgets pthread)

  CARGO_TERM_COLOR: <?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!-- Created with Inkscape (http://www.inkscape.org/) -->

<svg
   xmlns:dc="http://purl.org/dc/elements/1.1/"
   xmlns:cc="http://creativecommons.org/ns#"
   xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
   xmlns:svg="http://www.w3.org/2000/svg"
   xmlns="http://www.w3.org/2000/svg"
   xmlns:sodipodi="http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd"
   xmlns:inkscape="http://www.inkscape.org/namespaces/inkscape"
   width="297mm"
   height="210mm"
   viewBox="0 0 297 210"
   version="1.1"
   id="svg8"
   sodipodi:docname="VP-Display-layout.svg"
   inkscape:version="0.92+devel (e96c682733, 2018-10-03)"
   inkscape:export-filename="/home/dwd/dev/riscv-vp/vp-display/VP-Display-layout.svg.png"
   inkscape:export-xdpi="100"
   inkscape:export-ydpi="100">
  <defs
     id="defs2">
    <marker
       inkscape:stockid="Arrow2Lstart"
       orient="auto"
       refY="0.0"
       refX="0.0"
       id="marker1602"
       style="overflow:visible"
       inkscape:isstock="true">
      <path
         id="path1174"
         style="fill-rule:evenodd;stroke-width:0.625;stroke-linejoin:round;stroke:#000000;stroke-opacity:1;fill:#000000;fill-opacity:1"
         d="M 8.7185878,4.0337352 L -2.2072895,0.016013256 L 8.7185884,-4.0017078 C 6.9730900,-1.6296469 6.9831476,1.6157441 8.7185878,4.0337352 z "
         transform="scale(1.1) translate(1,0)" />
    </marker>
    <marker
       inkscape:stockid="Arrow1Lstart"
       orient="auto"
       refY="0.0"
       refX="0.0"
       id="Arrow1Lstart"
       style="overflow:visible"
       inkscape:isstock="true">
      <path
         id="path1156"
         d="M 0.0,0.0 L 5.0,-5.0 L -12.5,0.0 L 5.0,5.0 L 0.0,0.0 z "
         style="fill-rule:evenodd;stroke:#000000;stroke-width:1pt;stroke-opacity:1;fill:#000000;fill-opacity:1"
         transform="scale(0.8) translate(12.5,0)" />
    </marker>
    <marker
       inkscape:stockid="Arrow1Lend"
       orient="auto"
       refY="0.0"
       refX="0.0"
       id="marker1419"
       style="overflow:visible;"
       inkscape:isstock="true">
      <path
         id="path1159"
         d="M 0.0,0.0 L 5.0,-5.0 L -12.5,0.0 L 5.0,5.0 L 0.0,0.0 z "
         style="fill-rule:evenodd;stroke:#000000;stroke-width:1pt;stroke-opacity:1;fill:#000000;fill-opacity:1"
         transform="scale(0.8) rotate(180) translate(12.5,0)" />
    </marker>
    <marker
       inkscape:isstock="true"
       style="overflow:visible"
       id="marker2693"
       refX="0.0"
       refY="0.0"
       orient="auto"
       inkscape:stockid="Arrow2Lstart">
      <path
         transform="scale(1.1) translate(1,0)"
         d="M 8.7185878,4.0337352 L -2.2072895,0.016013256 L 8.7185884,-4.0017078 C 6.9730900,-1.6296469 6.9831476,1.6157441 8.7185878,4.0337352 z "
         style="fill-rule:evenodd;stroke-width:0.625;stroke-linejoin:round;stroke:#000000;stroke-opacity:1;fill:#000000;fill-opacity:1"
         id="path2691" />
    </marker>
    <marker
       inkscape:isstock="true"
       style="overflow:visible"
       id="marker2683"
       refX="0.0"
       refY="0.0"
       orient="auto"
       inkscape:stockid="Arrow2Lstart">
      <path
         transform="scale(1.1) translate(1,0)"
         d="M 8.7185878,4.0337352 L -2.2072895,0.016013256 L 8.7185884,-4.0017078 C 6.9730900,-1.6296469 6.9831476,1.6157441 8.7185878,4.0337352 z "
         style="fill-rule:evenodd;stroke-width:0.625;stroke-linejoin:round;stroke:#000000;stroke-opacity:1;fill:#000000;fill-opacity:1"
         id="path2681" />
    </marker>
    <marker
       inkscape:isstock="true"
       style="overflow:visible"
       id="marker2559"
       refX="0.0"
       refY="0.0"
       orient="auto"
       inkscape:stockid="Arrow1Mstart">
      <path
         transform="scale(0.4) translate(10,0)"
         style="fill-rule:evenodd;stroke:#000000;stroke-width:1pt;stroke-opacity:1;fill:#000000;fill-opacity:1"
         d="M 0.0,0.0 L 5.0,-5.0 L -12.5,0.0 L 5.0,5.0 L 0.0,0.0 z "
         id="path2557" />
    </marker>
    <marker
       inkscape:isstock="true"
       style="overflow:visible"
       id="marker2459"
       refX="0.0"
       refY="0.0"
       orient="auto"
       inkscape:stockid="Arrow1Mstart">
      <path
         transform="scale(0.4) translate(10,0)"
         style="fill-rule:evenodd;stroke:#000000;stroke-width:1pt;stroke-opacity:1;fill:#000000;fill-opacity:1"
         d="M 0.0,0.0 L 5.0,-5.0 L -12.5,0.0 L 5.0,5.0 L 0.0,0.0 z "
         id="path2457" />
    </marker>
    <marker
       inkscape:stockid="Arrow2Lstart"
       orient="auto"
       refY="0.0"
       refX="0.0"
       id="Arrow2Lstart"
       style="overflow:visible"
       inkscape:isstock="true"
       inkscape:collect="always">
      <path
         id="path2221"
         style="fill-rule:evenodd;stroke-width:0.625;stroke-linejoin:round;stroke:#000000;stroke-opacity:1;fill:#000000;fill-opacity:1"
         d="M 8.7185878,4.0337352 L -2.2072895,0.016013256 L 8.7185884,-4.0017078 C 6.9730900,-1.6296469 6.9831476,1.6157441 8.7185878,4.0337352 z "
         transform="scale(1.1) translate(1,0)" />
    </marker>
    <marker
       inkscape:isstock="true"
       style="overflow:visible;"
       id="marker2117"
       refX="0.0"
       refY="0.0"
       orient="auto"
       inkscape:stockid="Arrow2Lend">
      <path
         transform="scale(1.1) rotate(180) translate(1,0)"
         d="M 8.7185878,4.0337352 L -2.2072895,0.016013256 L 8.7185884,-4.0017078 C 6.9730900,-1.6296469 6.9831476,1.6157441 8.7185878,4.0337352 z "
         style="fill-rule:evenodd;stroke-width:0.625;stroke-linejoin:round;stroke:#000000;stroke-opacity:1;fill:#000000;fill-opacity:1"
         id="path2115" />
    </marker>
    <marker
       inkscape:stockid="Arrow2Lend"
       orient="auto"
       refY="0.0"
       refX="0.0"
       id="marker1266"
       style="overflow:visible;"
       inkscape:isstock="true"
       inkscape:collect="always">
      <path
         id="path1024"
         style="fill-rule:evenodd;stroke-width:0.625;stroke-linejoin:round;stroke:#000000;stroke-opacity:1;fill:#000000;fill-opacity:1"
         d="M 8.7185878,4.0337352 L -2.2072895,0.016013256 L 8.7185884,-4.0017078 C 6.9730900,-1.6296469 6.9831476,1.6157441 8.7185878,4.0337352 z "
         transform="scale(1.1) rotate(180) translate(1,0)" />
    </marker>
    <marker
       inkscape:stockid="Arrow2Lend"
       orient="auto"
       refY="0.0"
       refX="0.0"
       id="Arrow2Lend"
       style="overflow:visible;"
       inkscape:isstock="true">
      <path
         id="path1037"
         style="fill-rule:evenodd;stroke-width:0.625;stroke-linejoin:round;stroke:#000000;stroke-opacity:1;fill:#000000;fill-opacity:1"
         d="M 8.7185878,4.0337352 L -2.2072895,0.016013256 L 8.7185884,-4.0017078 C 6.9730900,-1.6296469 6.9831476,1.6157441 8.7185878,4.0337352 z "
         transform="scale(1.1) rotate(180) translate(1,0)" />
    </marker>
    <marker
       inkscape:stockid="Arrow1Lend"
       orient="auto"
       refY="0.0"
       refX="0.0"
       id="marker4188"
       style="overflow:visible;"
       inkscape:isstock="true">
      <path
         id="path4186"
         d="M 0.0,0.0 L 5.0,-5.0 L -12.5,0.0 L 5.0,5.0 L 0.0,0.0 z "
         style="fill-rule:evenodd;stroke:#000000;stroke-width:1pt;stroke-opacity:1;fill:#000000;fill-opacity:1"
         transform="scale(0.8) rotate(180) translate(12.5,0)" />
    </marker>
    <marker
       inkscape:stockid="Arrow1Lend"
       orient="auto"
       refY="0.0"
       refX="0.0"
       id="Arrow1Lend"
       style="overflow:visible;"
       inkscape:isstock="true">
      <path
         id="path1019"
         d="M 0.0,0.0 L 5.0,-5.0 L -12.5,0.0 L 5.0,5.0 L 0.0,0.0 z "
         style="fill-rule:evenodd;stroke:#000000;stroke-width:1pt;stroke-opacity:1;fill:#000000;fill-opacity:1"
         transform="scale(0.8) rotate(180) translate(12.5,0)" />
    </marker>
    <marker
       inkscape:stockid="Arrow2Lend"
       orient="auto"
       refY="0"
       refX="0"
       id="Arrow2Lend-7"
       style="overflow:visible"
       inkscape:isstock="true">
      <path
         inkscape:connector-curvature="0"
         id="path1037-4"
         style="fill:#000000;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:0.625;stroke-linejoin:round;stroke-opacity:1"
         d="M 8.7185878,4.0337352 -2.2072895,0.01601326 8.7185884,-4.0017078 c -1.7454984,2.3720609 -1.7354408,5.6174519 -6e-7,8.035443 z"
         transform="matrix(-1.1,0,0,-1.1,-1.1,0)" />
    </marker>
    <marker
       inkscape:stockid="Arrow2Lend"
       orient="auto"
       refY="0"
       refX="0"
       id="marker1266-3"
       style="overflow:visible"
       inkscape:isstock="true">
      <path
         inkscape:connector-curvature="0"
         id="path1024-6"
         style="fill:#000000;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:0.625;stroke-linejoin:round;stroke-opacity:1"
         d="M 8.7185878,4.0337352 -2.2072895,0.01601326 8.7185884,-4.0017078 c -1.7454984,2.3720609 -1.7354408,5.6174519 -6e-7,8.035443 z"
         transform="matrix(-1.1,0,0,-1.1,-1.1,0)" />
    </marker>
    <marker
       inkscape:stockid="Arrow2Lend"
       orient="auto"
       refY="0"
       refX="0"
       id="marker1266-5"
       style="overflow:visible"
       inkscape:isstock="true">
      <path
         inkscape:connector-curvature="0"
         id="path1024-3"
         style="fill:#000000;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:0.625;stroke-linejoin:round;stroke-opacity:1"
         d="M 8.7185878,4.0337352 -2.2072895,0.01601326 8.7185884,-4.0017078 c -1.7454984,2.3720609 -1.7354408,5.6174519 -6e-7,8.035443 z"
         transform="matrix(-1.1,0,0,-1.1,-1.1,0)" />
    </marker>
    <marker
       inkscape:stockid="Arrow2Lend"
       orient="auto"
       refY="0"
       refX="0"
       id="marker1266-3-6"
       style="overflow:visible"
       inkscape:isstock="true">
      <path
         inkscape:connector-curvature="0"
         id="path1024-6-2"
         style="fill:#000000;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:0.625;stroke-linejoin:round;stroke-opacity:1"
         d="M 8.7185878,4.0337352 -2.2072895,0.01601326 8.7185884,-4.0017078 c -1.7454984,2.3720609 -1.7354408,5.6174519 -6e-7,8.035443 z"
         transform="matrix(-1.1,0,0,-1.1,-1.1,0)" />
    </marker>
    <marker
       inkscape:stockid="Arrow2Lend"
       orient="auto"
       refY="0"
       refX="0"
       id="marker1266-5-9"
       style="overflow:visible"
       inkscape:isstock="true">
      <path
         inkscape:connector-curvature="0"
         id="path1024-3-1"
         style="fill:#000000;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:0.625;stroke-linejoin:round;stroke-opacity:1"
         d="M 8.7185878,4.0337352 -2.2072895,0.01601326 8.7185884,-4.0017078 c -1.7454984,2.3720609 -1.7354408,5.6174519 -6e-7,8.035443 z"
         transform="matrix(-1.1,0,0,-1.1,-1.1,0)" />
    </marker>
    <marker
       inkscape:stockid="Arrow2Lend"
       orient="auto"
       refY="0"
       refX="0"
       id="marker1266-2"
       style="overflow:visible"
       inkscape:isstock="true">
      <path
         inkscape:connector-curvature="0"
         id="path1024-7"
         style="fill:#000000;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:0.625;stroke-linejoin:round;stroke-opacity:1"
         d="M 8.7185878,4.0337352 -2.2072895,0.01601326 8.7185884,-4.0017078 c -1.7454984,2.3720609 -1.7354408,5.6174519 -6e-7,8.035443 z"
         transform="matrix(-1.1,0,0,-1.1,-1.1,0)" />
    </marker>
    <marker
       inkscape:stockid="Arrow2Lend"
       orient="auto"
       refY="0"
       refX="0"
       id="marker1266-5-9-9"
       style="overflow:visible"
       inkscape:isstock="true">
      <path
         inkscape:connector-curvature="0"
         id="path1024-3-1-0"
         style="fill:#000000;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:0.625;stroke-linejoin:round;stroke-opacity:1"
         d="M 8.7185878,4.0337352 -2.2072895,0.01601326 8.7185884,-4.0017078 c -1.7454984,2.3720609 -1.7354408,5.6174519 -6e-7,8.035443 z"
         transform="matrix(-1.1,0,0,-1.1,-1.1,0)" />
    </marker>
    <marker
       inkscape:stockid="Arrow2Lend"
       orient="auto"
       refY="0"
       refX="0"
       id="Arrow2Lend-6"
       style="overflow:visible"
       inkscape:isstock="true">
      <path
         inkscape:connector-curvature="0"
         id="path1037-47"
         style="fill:#000000;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:0.625;stroke-linejoin:round;stroke-opacity:1"
         d="M 8.7185878,4.0337352 -2.2072895,0.01601326 8.7185884,-4.0017078 c -1.7454984,2.3720609 -1.7354408,5.6174519 -6e-7,8.035443 z"
         transform="matrix(-1.1,0,0,-1.1,-1.1,0)" />
    </marker>
    <marker
       inkscape:stockid="Arrow2Lend"
       orient="auto"
       refY="0"
       refX="0"
       id="Arrow2Lend-8"
       style="overflow:visible"
       inkscape:isstock="true">
      <path
         inkscape:connector-curvature="0"
         id="path1037-5"
         style="fill:#000000;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:0.625;stroke-linejoin:round;stroke-opacity:1"
         d="M 8.7185878,4.0337352 -2.2072895,0.01601326 8.7185884,-4.0017078 c -1.7454984,2.3720609 -1.7354408,5.6174519 -6e-7,8.035443 z"
         transform="matrix(-1.1,0,0,-1.1,-1.1,0)" />
    </marker>
    <marker
       inkscape:stockid="Arrow2Lend"
       orient="auto"
       refY="0"
       refX="0"
       id="Arrow2Lend-8-2"
       style="overflow:visible"
       inkscape:isstock="true">
      <path
         inkscape:connector-curvature="0"
         id="path1037-5-6"
         style="fill:#000000;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:0.625;stroke-linejoin:round;stroke-opacity:1"
         d="M 8.7185878,4.0337352 -2.2072895,0.01601326 8.7185884,-4.0017078 c -1.7454984,2.3720609 -1.7354408,5.6174519 -6e-7,8.035443 z"
         transform="matrix(-1.1,0,0,-1.1,-1.1,0)" />
    </marker>
  </defs>
  <sodipodi:namedview
     id="base"
     pagecolor="#ffffff"
     bordercolor="#666666"
     borderopacity="1.0"
     inkscape:pageopacity="0.0"
     inkscape:pageshadow="2"
     inkscape:zoom="0.60800715"
     inkscape:cx="122.5113"
     inkscape:cy="104.55609"
     inkscape:document-units="mm"
     inkscape:current-layer="layer1"
     showgrid="false"
     inkscape:pagecheckerboard="true"
     inkscape:snap-bbox="true"
     inkscape:bbox-paths="true"
     inkscape:snap-bbox-edge-midpoints="true"
     inkscape:bbox-nodes="true"
     inkscape:snap-bbox-midpoints="true"
     inkscape:document-rotation="0"
     inkscape:window-width="1920"
     inkscape:window-height="1053"
     inkscape:window-x="1920"
     inkscape:window-y="0"
     inkscape:window-maximized="1" />
  <metadata
     id="metadata5">
    <rdf:RDF>
      <cc:Work
         rdf:about="">
        <dc:format>image/svg+xml</dc:format>
        <dc:type
           rdf:resource="http://purl.org/dc/dcmitype/StillImage" />
        <dc:title />
      </cc:Work>
    </rdf:RDF>
  </metadata>
  <g
     inkscape:label="Layer 1"
     inkscape:groupmode="layer"
     id="layer1"
     transform="translate(0,-87)">
    <g
       id="g121"
       transform="matrix(1.0207115,0,0,1.012175,4.3205134,-3.1117991)">
      <rect
         style="fill:none;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:0.971382;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1;image-rendering:auto"
         y="106.18889"
         x="112.14192"
         height="185.47688"
         width="165.57283"
         id="rect3729-3" />
      <rect
         y="120.02941"
         x="118.77986"
         height="8.9131947"
         width="26.465324"
         id="rect4555"
         style="fill:none;fill-opacity:1;fill-rule:evenodd;stroke:#730000;stroke-width:0.96652;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1" />
      <rect
         y="120.02985"
         x="149.26895"
         height="8.9123001"
         width="26.464432"
         id="rect4557"
         style="fill:none;fill-opacity:1;fill-rule:evenodd;stroke:#730000;stroke-width:0.967414;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1" />
      <rect
         y="144.38206"
         x="118.79264"
         height="44.484093"
         width="56.928413"
         id="rect4559"
         style="fill:none;fill-opacity:1;fill-rule:evenodd;stroke:#730000;stroke-width:1;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1" />
      <rect
         y="192.1544"
         x="118.79264"
         height="44.484093"
         width="56.928413"
         id="rect4559-6"
         style="fill:none;fill-opacity:1;fill-rule:evenodd;stroke:#730000;stroke-width:1;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1" />
      <rect
         y="239.92674"
         x="118.79264"
         height="44.484093"
         width="56.928413"
         id="rect4559-7"
         style="fill:none;fill-opacity:1;fill-rule:evenodd;stroke:#730000;stroke-width:1;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1" />
      <text
         id="text4586"
         y="125.12725"
         x="204.61108"
         style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:5.55318px;line-height:1.25;font-family:Cursor;-inkscape-font-specification:Cursor;letter-spacing:0px;word-spacing:0px;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.00983832;stroke-miterlimit:4;stroke-dasharray:0.00983832, 0.0295149;stroke-dashoffset:0;stroke-opacity:1"
         xml:space="preserve"><tspan
           id="tspan4588"
           style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:5.55318px;line-height:0.45;font-family:'Latin Modern Mono';-inkscape-font-specification:'Latin Modern Mono';stroke:none;stroke-width:0.00983832;stroke-miterlimit:4;stroke-dasharray:0.00983832, 0.0295149;stroke-dashoffset:0;stroke-opacity:1"
           y="125.12725"
           x="204.61108"
           sodipodi:role="line">HW-Accelerations:</tspan><tspan
           style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:5.55318px;line-height:0.45;font-family:'Latin Modern Mono';-inkscape-font-specification:'Latin Modern Mono';stroke:none;stroke-width:0.00983832;stroke-miterlimit:4;stroke-dasharray:0.00983832, 0.0295149;stroke-dashoffset:0;stroke-opacity:1"
           y="132.06873"
           x="204.61108"
           sodipodi:role="line"
           id="tspan3433"> <tspan
   style="font-size:4.51196px"
   id="tspan3492">ApplyBuffer,FillFrame,...</tspan></tspan><tspan
           id="tspan4592"
           style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:5.55318px;line-height:0.45;font-family:'Latin Modern Mono';-inkscape-font-specification:'Latin Modern Mono';stroke:none;stroke-width:0.00983832;stroke-miterlimit:4;stroke-dasharray:0.00983832, 0.0295149;stroke-dashoffset:0;stroke-opacity:1"
           y="139.01021"
           x="204.61108"
           sodipodi:role="line">Direct IO:</tspan><tspan
           rotate="0 0 0"
           style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:5.55318px;line-height:0.45;font-family:'Latin Modern Mono';-inkscape-font-specification:'Latin Modern Mono';stroke:none;stroke-width:0.00983832;stroke-miterlimit:4;stroke-dasharray:0.00983832, 0.0295149;stroke-dashoffset:0;stroke-opacity:1"
           y="145.95168"
           x="204.61108"
           sodipodi:role="line"
           id="tspan3435"> <tspan
   style="font-size:4.85903px;stroke:none;stroke-width:0.00983832;stroke-miterlimit:4;stroke-dasharray:0.00983832, 0.0295149;stroke-dashoffset:0;stroke-opacity:1"
   id="tspan3439">Read/Write Pixel</tspan></tspan></text>
      <text
         xml:space="preserve"
         style="font-style:normal;font-weight:normal;font-size:10.4122px;font-family:sans-serif;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.260305"
         x="15.711744"
         y="102.29607"
         id="text125"
         transform="scale(0.99580954,1.0042081)"><tspan
           sodipodi:role="line"
           id="tspan123"
           x="15.711744"
           y="102.29607"
           style="stroke-width:0.260305">VP-Display</tspan></text>
      <text
         xml:space="preserve"
         style="font-style:normal;font-weight:normal;font-size:10.4122px;font-family:sans-serif;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.260305"
         x="171.27893"
         y="102.29607"
         id="text129"
         transform="scale(0.99580954,1.0042081)"><tspan
           sodipodi:role="line"
           id="tspan127"
           x="171.27893"
           y="102.29607"
           style="stroke-width:0.260305">Risc-V VP</tspan></text>
      <text
         xml:space="preserve"
         style="font-style:normal;font-weight:normal;font-size:6.24733px;font-family:sans-serif;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.260305"
         x="125.869"
         y="115.17609"
         id="text133"
         transform="scale(0.99580954,1.0042081)"><tspan
           sodipodi:role="line"
           id="tspan131"
           x="125.869"
           y="115.17609"
           style="font-size:6.24733px;stroke-width:0.260305">Graphics Card</tspan></text>
      <text
         xml:space="preserve"
         style="font-style:normal;font-weight:normal;font-size:6.24733px;font-family:sans-serif;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.260305"
         x="224.29309"
         y="115.17609"
         id="text137"
         transform="scale(0.99580954,1.0042081)"><tspan
           sodipodi:role="line"
           id="tspan135"
           x="224.29309"
           y="115.17609"
           style="font-size:6.24733px;stroke-width:0.260305">Library</tspan></text>
      <text
         xml:space="preserve"
         style="font-style:normal;font-weight:normal;font-size:3.47074px;font-family:sans-serif;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.260305"
         x="120.60262"
         y="125.25826"
         id="text956"
         transform="scale(0.99580954,1.0042081)"><tspan
           sodipodi:role="line"
           id="tspan954"
           x="120.60262"
           y="125.25826"
           style="font-size:3.47074px;stroke-width:0.260305">framenumber</tspan></text>
      <text
         xml:space="preserve"
         style="font-style:normal;font-weight:normal;font-size:3.47074px;font-family:sans-serif;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.260305"
         x="154.90385"
         y="125.25826"
         id="text960"
         transform="scale(0.99580954,1.0042081)"><tspan
           sodipodi:role="line"
           id="tspan958"
           x="154.90385"
           y="125.25826"
           style="font-size:3.47074px;stroke-width:0.260305">operation</tspan></text>
      <text
         xml:space="preserve"
         style="font-style:normal;font-weight:normal;font-size:6.24733px;font-family:sans-serif;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.260305"
         x="126.1039"
         y="151.47054"
         id="text964"
         transform="scale(0.99580954,1.0042081)"><tspan
           sodipodi:role="line"
           id="tspan962"
           x="126.1039"
           y="151.47054"
           style="font-size:6.24733px;stroke-width:0.260305">Framebuffer A</tspan></text>
      <path
         style="fill:none;stroke:#000000;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1;marker-end:url(#Arrow2Lend)"
         d="m 186.79692,123.45998 c -5.79748,0 -1.25271,1.10257 -9.1784,1.10257"
         id="path1014"
         inkscape:connector-curvature="0"
         sodipodi:nodetypes="cc" />
      <g
         style="opacity:1"
         id="g90"
         transform="matrix(1.0086454,0,0,0.98781815,-10.115798,17.029541)">
        <rect
           style="fill:none;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:1;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1;image-rendering:auto"
           y="90.267258"
           x="9.9590015"
           height="93.968872"
           width="86.432549"
           id="rect3729" />
      </g>
      <rect
         y="126.62711"
         x="7.5003734"
         height="58.347027"
         width="70.090843"
         id="rect4553"
         style="opacity:1;fill:none;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:0.998177;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1" />
      <path
         style="opacity:1;fill:none;stroke:#00ad00;stroke-width:0.949397;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1"
         d="m 38.833875,176.43381 c 0.04411,-1.1419 -0.07072,-2.2888 0,-3.42986 0.04389,-0.70824 0.215036,-1.40438 0.261845,-2.11243 0.06967,-1.05389 -0.08698,-2.11609 0,-3.16866 0.05125,-0.62023 0.20597,-1.22856 0.261846,-1.84838 0.05532,-0.6136 0,-1.23226 0,-1.84839 0,-0.26405 -0.03703,-0.53076 0,-0.79216 0.05089,-0.35926 0.217212,-0.69611 0.261848,-1.05622 0.04331,-0.34935 -0.04937,-0.70768 0,-1.05622 0.03903,-0.27554 0.21647,-0.51761 0.261846,-0.79216 0.05429,-0.3285 0,-1.43682 0,-1.84838 0,-0.17598 0,-0.35196 0,-0.52794 0,-0.0665 0.0359,-0.14379 0,-0.19962 -0.02328,-0.0362 -0.08715,-0.0116 -0.128366,0 -0.58132,0.16361 -1.061613,0.3152 -1.51105,0.76843 -0.138006,0.13917 -0.124376,0.3884 -0.261845,0.52811 -0.06054,0.0615 -0.171881,0 -0.257821,0 -0.15888,0 -0.387429,0.13257 -0.476641,0 -0.131471,-0.19538 0,-0.47223 0,-0.70834 0,-0.70161 0,-1.40322 0,-2.10483 0,-0.17604 0.07231,-0.36789 0,-0.52811 -0.0329,-0.0729 -0.16499,-0.0308 -0.238322,0 -0.518076,0.21766 -0.986112,0.55277 -1.516675,0.73733 -0.940772,0.32725 -1.896149,0.6433 -2.88031,0.79217 -1.029273,0.15569 -3.142155,0.5784 -3.142155,-1.32028 0,-2.3243 1.236412,-3.4715 3.142155,-4.48893 0.02149,-0.0115 2.356616,-1.32026 2.356616,-0.0199 0,0.36289 -0.03147,0.77738 -0.261845,1.05615 -0.249893,0.30239 -0.677079,0.40363 -1.047385,0.52811 -0.165607,0.0557 -0.473531,0.16861 -0.523692,0 -0.485283,-1.63125 -0.419782,-4.17863 0.785537,-5.54515 0.806871,-0.91478 1.998685,-1.4508 3.146376,-1.85003 1.408665,-0.49 3.635772,-1.15389 3.197772,1.05458 -0.09981,0.50326 -0.3686,1.02596 -0.785539,1.32027 -0.861005,0.60779 -2.737084,0.79438 -2.88031,0 -0.143947,-0.79839 0.07733,-1.70129 0.523694,-2.37649 0.651362,-0.98528 1.611542,-1.76724 2.618461,-2.37649 0.781392,-0.47279 1.719612,-0.64109 2.618465,-0.79217 0.192513,-0.0324 0.523693,0.0673 0.523693,0.26406 0,0.72582 -0.225861,1.45168 -0.523693,2.11244 -0.153235,0.33996 -0.415249,0.7886 -0.78554,0.79216 -0.272842,0.003 -0.297998,-0.50936 -0.261846,-0.78209 0.306659,-2.31344 0.929217,-4.81947 3.453929,-5.84277 0.835246,-0.33854 1.989796,-0.0977 2.651257,0.51798 1.596441,1.48606 1.609396,5.65477 1.309234,7.361 -0.127148,0.72273 -0.656075,1.31644 -1.074748,1.91689 -0.0502,0.072 -0.289447,0.0835 -0.261846,0 0.271838,-0.82239 0.452283,-1.75092 1.047385,-2.37649 1.149451,-1.2083 2.52811,-2.23584 4.026526,-2.95717 0.991542,-0.47733 2.223035,-0.93665 3.251344,-0.54637 2.158359,0.81916 1.788882,4.91013 1.300985,6.17458 -0.387847,1.00515 -1.313948,1.72232 -2.128587,2.4212 -0.358006,0.30713 -0.926754,0.80361 -1.309233,0.52811 -0.517063,-0.37245 -0.807861,-1.27525 -0.523692,-1.84839 0.525146,-1.05914 1.547841,-1.89008 2.618462,-2.37649 1.165418,-0.52948 2.520237,-0.59233 3.797346,-0.53837 1.79046,0.0756 3.90567,0.9796 2.410572,3.44118 -1.408365,2.31879 -4.901585,3.58793 -7.283687,4.17534 -0.628663,0.15502 -1.873168,0.67347 -1.941409,0.0245 -0.0742,-0.70567 1.161899,-0.84253 1.832925,-1.05622 1.012547,-0.32244 2.080324,-0.52811 3.142155,-0.52811 1.601864,0 0.57583,3.05202 0.523694,3.16866 -1.288377,2.88245 -3.029151,3.37231 -6.036338,3.71058 -0.312761,0.0352 -0.94745,-0.25598 -0.78554,-0.52811 0.348626,-0.58595 4.181329,-1.58306 4.189543,-1.58433 0.658623,-0.10218 1.365027,0.5898 0.785537,1.32027 -1.605969,2.0244 -2.826027,1.74708 -5.236925,1.84839 -1.046478,0.044 -2.096383,0.0586 -3.142156,0 -1.018345,-0.0571 -1.832924,0.0443 -1.832924,-1.03292 0,-0.0574 0,0.11471 0,0.17206 0,0.0768 0,0.15367 0,0.2305 0,0.79217 0,1.58433 0,2.3765 0,1.23225 0,2.46451 0,3.69676 0,1.05622 -0.0523,2.11375 0,3.16866 0.03514,0.70874 0.14616,1.41246 0.261848,2.11243 0.07152,0.43273 0.36296,1.07449 0.261845,1.58433 -0.162618,0.81995 -0.447474,1.53101 -0.523693,2.37649 -0.04741,0.52594 0,1.05622 0,1.58433 0,0.088 0.03806,0.18484 0,0.26406 -0.05057,0.10525 -0.164037,0.16539 -0.246055,0.24809"
         id="path141"
         inkscape:connector-curvature="0" />
      <path
         style="opacity:1;fill:none;stroke:#000000;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1"
         d="m 10.594045,173.19068 c 5.087102,0 10.174204,0 15.261304,0 6.801037,0 13.602704,-0.0935 20.403109,0 3.712272,0.051 7.419152,0.30412 11.129553,0.43342 0.435511,0.0152 0.878001,-0.0754 1.307315,0 1.422393,0.7172 0.750672,0.24729 0.448514,0.35209 -0.181077,0.0628 -0.360462,0.14508 -0.511356,0.26397 -0.07178,0.0566 0.190057,0.0373 0.27329,0 0.692739,-0.31071 1.362629,-0.74485 2.06824,-1.02948 0.727286,-0.29337 1.863961,-0.66535 2.618464,-0.79217 0.323868,-0.0544 1.217946,0 1.571077,0 1.032918,0 2.065834,0 3.09875,0 0.523691,0 1.047385,0 1.571076,0 0.872822,0 1.748213,-0.0675 2.618464,0 0.275198,0.0214 0.523694,0.17604 0.78554,0.26406"
         id="path139"
         inkscape:connector-curvature="0" />
      <path
         style="opacity:1;fill:none;stroke:#009800;stroke-width:0.722696;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:0.525521"
         d="m 144.15951,280.59794 c 0.0336,-0.86925 -0.0538,-1.74228 0,-2.61088 0.0334,-0.53912 0.16368,-1.06904 0.19931,-1.60801 0.0531,-0.80224 -0.0662,-1.61081 0,-2.41204 0.0391,-0.47213 0.15679,-0.93521 0.19933,-1.40702 0.0421,-0.46709 0,-0.93802 0,-1.40703 0,-0.201 -0.0282,-0.40402 0,-0.603 0.0387,-0.27348 0.16534,-0.52989 0.19932,-0.80402 0.033,-0.26593 -0.0376,-0.53869 0,-0.80401 0.0297,-0.20974 0.16478,-0.39401 0.19932,-0.603 0.0413,-0.25006 0,-1.09374 0,-1.40702 0,-0.13396 0,-0.26792 0,-0.40188 0,-0.0506 0.0273,-0.10946 0,-0.15195 -0.0177,-0.0276 -0.0664,-0.009 -0.0977,0 -0.44251,0.12454 -0.80812,0.23993 -1.15024,0.58494 -0.10505,0.10594 -0.0947,0.29565 -0.19932,0.402 -0.0461,0.0468 -0.13084,0 -0.19626,0 -0.12094,0 -0.29492,0.10092 -0.36282,0 -0.10008,-0.14872 0,-0.35947 0,-0.5392 0,-0.53407 0,-1.06815 0,-1.60223 0,-0.134 0.055,-0.28004 0,-0.402 -0.025,-0.0555 -0.1256,-0.0234 -0.18142,0 -0.39437,0.16568 -0.75064,0.42077 -1.15452,0.56126 -0.71613,0.24911 -1.44338,0.48969 -2.19254,0.60302 -0.7835,0.11851 -2.39186,0.44028 -2.39186,-1.00502 0,-1.7693 0.94118,-2.64257 2.39186,-3.41705 0.0164,-0.009 1.7939,-1.00501 1.7939,-0.0152 0,0.27624 -0.024,0.59175 -0.19933,0.80396 -0.19022,0.23018 -0.5154,0.30725 -0.79728,0.402 -0.12606,0.0424 -0.36046,0.12835 -0.39864,0 -0.36941,-1.24173 -0.31955,-3.18084 0.59796,-4.22106 0.6142,-0.69634 1.52143,-1.10437 2.39508,-1.40827 1.0723,-0.373 2.76761,-0.87836 2.43419,0.80276 -0.076,0.38309 -0.28058,0.78098 -0.59796,1.00501 -0.65541,0.46266 -2.08352,0.6047 -2.19254,0 -0.10957,-0.60774 0.0588,-1.29505 0.39864,-1.80902 0.49583,-0.75001 1.22674,-1.34525 1.99322,-1.80902 0.59481,-0.3599 1.309,-0.48801 1.99322,-0.60302 0.14655,-0.0247 0.39865,0.0512 0.39865,0.20101 0,0.55251 -0.17194,1.10504 -0.39865,1.60802 -0.11664,0.25879 -0.3161,0.6003 -0.59797,0.60301 -0.20769,0.002 -0.22684,-0.38773 -0.19932,-0.59534 0.23344,-1.76103 0.70733,-3.66866 2.62919,-4.44762 0.6358,-0.2577 1.51467,-0.0744 2.01818,0.3943 1.21524,1.13121 1.2251,4.30451 0.99662,5.60332 -0.0968,0.55015 -0.49942,1.00209 -0.81812,1.45917 -0.0382,0.0548 -0.22034,0.0636 -0.19933,0 0.20693,-0.62602 0.34429,-1.33283 0.79729,-1.80903 0.87499,-0.91978 1.92444,-1.70196 3.06506,-2.25105 0.75478,-0.36335 1.69222,-0.71299 2.47498,-0.4159 1.64298,0.62356 1.36173,3.73767 0.99033,4.70019 -0.29523,0.76514 -1.0002,1.31106 -1.62031,1.84306 -0.27252,0.23379 -0.70547,0.61172 -0.99662,0.40201 -0.39359,-0.28352 -0.61495,-0.97074 -0.39864,-1.40703 0.39975,-0.80623 1.17824,-1.43876 1.99322,-1.80902 0.88714,-0.40305 1.91845,-0.45089 2.89061,-0.40982 1.36292,0.0575 2.97306,0.74569 1.83496,2.61949 -1.07207,1.7651 -3.73116,2.73119 -5.54447,3.17834 -0.47854,0.118 -1.42588,0.51265 -1.47782,0.0186 -0.0565,-0.53716 0.88445,-0.64134 1.39525,-0.80401 0.77077,-0.24544 1.58357,-0.402 2.39186,-0.402 1.21937,0 0.43833,2.32325 0.39864,2.41203 -0.98073,2.19417 -2.30584,2.56706 -4.59496,2.82456 -0.23808,0.0268 -0.72122,-0.19486 -0.59797,-0.40201 0.26539,-0.44603 3.1829,-1.20505 3.18915,-1.20601 0.50136,-0.0778 1.03909,0.44896 0.59797,1.00501 -1.22249,1.541 -2.15122,1.3299 -3.98643,1.40702 -0.7966,0.0335 -1.59581,0.0446 -2.39187,0 -0.77518,-0.0435 -1.39525,0.0337 -1.39525,-0.78627 0,-0.0437 0,0.0873 0,0.13097 0,0.0585 0,0.11698 0,0.17546 0,0.60302 0,1.20602 0,1.80903 0,0.93801 0,1.87602 0,2.81404 0,0.80401 -0.0398,1.60902 0,2.41203 0.0267,0.53951 0.11126,1.07519 0.19932,1.60803 0.0544,0.32939 0.27629,0.81791 0.19932,1.20601 -0.12379,0.62416 -0.34062,1.16543 -0.39864,1.80902 -0.0361,0.40036 0,0.80402 0,1.20603 0,0.067 0.029,0.14069 0,0.201 -0.0385,0.0801 -0.12487,0.1259 -0.1873,0.18886"
         id="path141-7"
         inkscape:connector-curvature="0" />
      <path
         style="opacity:1;fill:none;stroke:#000000;stroke-width:0.198149px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1"
         d="m 122.66287,278.1292 c 3.87239,0 7.74477,0 11.61716,0 5.17707,0 10.35461,-0.0712 15.53119,0 2.82585,0.0388 5.64759,0.23151 8.47201,0.32993 0.33152,0.0116 0.66835,-0.0574 0.99515,0 1.08275,0.54594 0.57142,0.18824 0.34141,0.26802 -0.13783,0.0478 -0.27438,0.11044 -0.38924,0.20094 -0.0547,0.0431 0.14466,0.0284 0.20803,0 0.52732,-0.23652 1.03726,-0.567 1.57438,-0.78366 0.55362,-0.22331 1.41888,-0.50648 1.99321,-0.60302 0.24654,-0.0414 0.92713,0 1.19594,0 0.78627,0 1.57254,0 2.35882,0 0.39864,0 0.79728,0 1.19592,0 0.66442,0 1.33078,-0.0514 1.99323,0 0.20948,0.0163 0.39864,0.13401 0.59796,0.20102"
         id="path139-5"
         inkscape:connector-curvature="0" />
      <path
         style="opacity:1;fill:none;stroke:#000000;stroke-width:0.654248;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1"
         d="M -0.02613637,112.24182 H 86.919685"
         id="path974"
         inkscape:connector-curvature="0"
         sodipodi:nodetypes="cc" />
      <path
         style="opacity:1;fill:none;stroke:#000000;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1"
         d="m 66.951631,112.26195 v -5.9609"
         id="path976"
         inkscape:connector-curvature="0" />
      <path
         style="opacity:1;fill:none;stroke:#000000;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1"
         d="m 73.656278,112.26195 v -5.81067"
         id="path978"
         inkscape:connector-curvature="0" />
      <path
         style="opacity:1;fill:none;stroke:#000000;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1"
         d="m 80.360924,112.26195 v -5.58842"
         id="path980"
         inkscape:connector-curvature="0" />
      <text
         xml:space="preserve"
         style="font-style:normal;font-weight:normal;font-size:4.16489px;font-family:sans-serif;opacity:1;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.260305"
         x="82.674248"
         y="110.26443"
         id="text984"
         transform="scale(0.99580954,1.0042081)"><tspan
           sodipodi:role="line"
           id="tspan982"
           x="82.674248"
           y="110.26443"
           style="font-size:4.16489px;stroke-width:0.260305">x</tspan></text>
      <text
         xml:space="preserve"
         style="font-style:normal;font-weight:normal;font-size:4.16489px;font-family:sans-serif;opacity:1;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.260305"
         x="76.175323"
         y="109.3183"
         id="text984-2"
         transform="scale(0.99580954,1.0042081)"><tspan
           sodipodi:role="line"
           id="tspan982-5"
           x="76.175323"
           y="109.3183"
           style="font-size:4.16489px;stroke-width:0.260305">_</tspan></text>
      <path
         style="opacity:1;fill:none;stroke:#000000;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1"
         d="m 68.835253,110.78806 h 2.776292 v -2.36565 h -2.778163 z"
         id="path1012"
         inkscape:connector-curvature="0" />
      <text
         id="text4586-3"
         y="120.06895"
         x="37.555855"
         style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:5.64444px;line-height:1.25;font-family:Cursor;-inkscape-font-specification:Cursor;letter-spacing:0px;word-spacing:0px;opacity:1;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.264582"
         xml:space="preserve"><tspan
           id="tspan4592-7"
           style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:5.64444px;line-height:0.45;font-family:'Latin Modern Mono';-inkscape-font-specification:'Latin Modern Mono';stroke-width:0.264582"
           y="120.06895"
           x="37.555855"
           sodipodi:role="line">getNewestFrame()</tspan></text>
      <text
         xml:space="preserve"
         style="font-style:normal;font-weight:normal;font-size:6.24733px;font-family:sans-serif;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.260305"
         x="129.20317"
         y="247.07974"
         id="text972"
         transform="scale(0.99580954,1.0042081)"><tspan
           sodipodi:role="line"
           id="tspan970"
           x="129.20317"
           y="247.07974"
           style="font-size:6.24733px;stroke-width:0.260305">Background</tspan></text>
      <text
         id="text4586-3-5"
         y="100.16684"
         x="-222.68347"
         style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:5.64444px;line-height:1.25;font-family:Cursor;-inkscape-font-specification:Cursor;letter-spacing:0px;word-spacing:0px;fill:#730000;fill-opacity:1;stroke:none;stroke-width:0.264582"
         xml:space="preserve"
         transform="matrix(0,-1.0084338,0.99163673,0,0,0)"><tspan
           id="tspan4592-7-3"
           style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:5.64444px;line-height:0.45;font-family:Cursor;-inkscape-font-specification:Cursor;fill:#730000;fill-opacity:1;stroke-width:0.264582"
           y="100.16684"
           x="-222.68347"
           sodipodi:role="line">IPC: sharedMemory</tspan></text>
      <text
         id="text4586-3-5-3"
         y="195.74734"
         x="-222.75238"
         style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:5.64444px;line-height:1.25;font-family:Cursor;-inkscape-font-specification:Cursor;letter-spacing:0px;word-spacing:0px;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.264582"
         xml:space="preserve"
         transform="matrix(0,-1.0084338,0.99163674,0,0,0)"><tspan
           id="tspan4592-7-3-4"
           style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:5.64444px;line-height:0.45;font-family:Cursor;-inkscape-font-specification:Cursor;fill:#000000;fill-opacity:1;stroke-width:0.264582"
           y="195.74734"
           x="-222.75238"
           sodipodi:role="line">Memory Mapped IO</tspan></text>
      <g
         id="g1607"
         transform="translate(2.0384039,-1.0052618)">
        <path
           inkscape:connector-curvature="0"
           id="path1587"
           d="m 14.654395,156.3148 c 0.03086,-0.21784 -0.01811,-0.46387 0.09258,-0.6535 0.210915,-0.36136 0.51464,-0.66709 0.834545,-0.93494 0.814735,-0.68217 1.783373,-1.2107 2.7872,-1.54723 0.410363,-0.13757 0.833709,-0.25283 1.264272,-0.29345 0.285209,-0.0269 0.57532,0.0282 0.854346,0.0935 0.54702,0.12814 0.98993,0.33503 1.296073,0.84022 0.209186,0.34519 0.377663,0.72497 0.462882,1.12028 0.07211,0.3345 0.03218,0.68617 0,1.02693 -0.02984,0.31593 -0.08973,0.63116 -0.185152,0.93358 -0.292505,0.92706 -0.579544,1.46479 -1.296073,2.14722 -0.453148,0.43158 -1.177044,0.74826 -1.758958,0.93358 -0.707129,0.22518 -1.278625,0.25621 -2.036685,0.18671 -0.283337,-0.026 -0.569033,-0.0802 -0.833191,-0.18671 -0.449442,-0.1813 -0.656571,-0.46696 -0.925765,-0.84022 -0.355243,-0.49258 -0.367751,-0.50638 -0.55546,-1.02693 -0.233946,-0.64878 -0.185153,-0.40996 -0.185153,-1.02693 0,-0.12448 -0.02892,-0.25242 0,-0.37343 0.03408,-0.1426 0.123028,-0.26583 0.184542,-0.39873 z"
           style="fill:none;stroke:#000092;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1" />
        <path
           inkscape:connector-curvature="0"
           id="path1589"
           d="m 18.010729,160.31885 c 0.03086,0.3674 0.04686,0.73638 0.09258,1.1022 0.06172,0.37344 0.123436,0.74686 0.185155,1.12029 0,0.12448 -0.01364,0.24972 0,0.37343 0.01737,0.15771 0.0752,0.30908 0.09258,0.46679 0.02302,0.20895 0,0.52208 0,0.74686 0,0.59126 0,1.18253 0,1.77379 0,0.18672 -0.01421,0.37398 0,0.56015 0.04567,0.59864 0.141178,0.58116 0.09258,1.12028 0.03266,0.32945 -0.14612,0.61867 -0.185152,0.93358 -0.01532,0.12351 0.01745,0.2502 0,0.37343 -0.01379,0.0974 -0.07653,0.183 -0.09258,0.28007 -0.01522,0.0921 0,0.18672 0,0.28007 0,0.18672 0,0.37343 0,0.56015 0,0.37343 0,0.74686 0,1.12029 0,0.34231 0,0.68462 0,1.02693 0,0.12447 0,0.24895 0,0.37343 0,0.0622 0.03398,0.13476 0,0.18671 -0.03768,0.0576 -0.122142,0.0622 -0.18321,0.0934"
           style="fill:none;stroke:#000092;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1" />
        <path
           inkscape:connector-curvature="0"
           id="path1593"
           d="m 18.248815,163.85896 c -1.53536,-0.87272 -2.415761,-1.41768 -4.194631,-2.14689 -1.727333,-0.70808 -2.553291,-0.9287 -4.027558,-1.35306"
           style="fill:none;stroke:#000092;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1" />
        <path
           inkscape:connector-curvature="0"
           id="path1595"
           d="m 18.600566,172.04684 c 1.355555,1.11596 0.987512,0.73807 2.396867,2.32373 0.351503,0.39548 0.725028,0.77672 1.02024,1.21652 0.180323,0.26864 0.24895,0.59879 0.392977,0.88885 0.05277,0.10627 0.130517,0.19792 0.195777,0.29688"
           style="fill:none;stroke:#000092;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1" />
        <path
           inkscape:connector-curvature="0"
           id="path1597"
           d="m 17.77454,172.36016 c -0.987297,0.63788 -2.174925,0.86442 -3.183949,1.45094 -0.340902,0.19816 -0.577526,0.54149 -0.834207,0.84224 -0.479235,0.5615 -0.30419,0.41832 -0.487474,0.87769 -0.07222,0.18102 -0.167352,0.29982 -0.27773,0.46678"
           style="fill:none;stroke:#000092;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1" />
        <path
           inkscape:connector-curvature="0"
           id="path1599"
           d="m 19.11498,163.49801 c 1.707836,-1.30942 1.011331,-0.99818 2.708093,-1.62038 0.275136,-0.10089 0.546137,-0.22217 0.833214,-0.28007 0.181557,-0.0366 0.370306,0 0.555458,0 0.277731,0 0.555461,0 0.833191,0"
           style="fill:none;stroke:#000092;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1" />
      </g>
      <g
         id="g1607-3"
         transform="matrix(0.70109934,0,0,0.70109934,117.26133,101.95117)">
        <path
           inkscape:connector-curvature="0"
           id="path1587-5"
           d="m 19.462494,160.87443 c 0.03086,-0.21784 -0.01811,-0.46387 0.09258,-0.6535 0.210915,-0.36136 0.51464,-0.66709 0.834545,-0.93494 0.814735,-0.68217 1.783373,-1.2107 2.7872,-1.54723 0.410363,-0.13757 0.833709,-0.25283 1.264272,-0.29345 0.285209,-0.0269 0.57532,0.0282 0.854346,0.0935 0.54702,0.12814 0.98993,0.33503 1.296073,0.84022 0.209186,0.34519 0.377663,0.72497 0.462882,1.12028 0.07211,0.3345 0.03218,0.68617 0,1.02693 -0.02984,0.31593 -0.08973,0.63116 -0.185152,0.93358 -0.292505,0.92706 -0.579544,1.46479 -1.296073,2.14722 -0.453148,0.43158 -1.177044,0.74826 -1.758958,0.93358 -0.707129,0.22518 -1.278625,0.25621 -2.036685,0.18671 -0.283337,-0.026 -0.569033,-0.0802 -0.833191,-0.18671 -0.449442,-0.1813 -0.656571,-0.46696 -0.925765,-0.84022 -0.355243,-0.49258 -0.367751,-0.50638 -0.55546,-1.02693 -0.233946,-0.64878 -0.185153,-0.40996 -0.185153,-1.02693 0,-0.12448 -0.02892,-0.25242 0,-0.37343 0.03408,-0.1426 0.123028,-0.26583 0.184542,-0.39873 z"
           style="fill:none;stroke:#000092;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1" />
        <path
           inkscape:connector-curvature="0"
           id="path1589-6"
           d="m 22.818828,164.87848 c 0.03086,0.3674 0.04686,0.73638 0.09258,1.1022 0.06172,0.37344 0.123436,0.74686 0.185155,1.12029 0,0.12448 -0.01364,0.24972 0,0.37343 0.01737,0.15771 0.0752,0.30908 0.09258,0.46679 0.02302,0.20895 0,0.52208 0,0.74686 0,0.59126 0,1.18253 0,1.77379 0,0.18672 -0.01421,0.37398 0,0.56015 0.04567,0.59864 0.141178,0.58116 0.09258,1.12028 0.03266,0.32945 -0.14612,0.61867 -0.185152,0.93358 -0.01532,0.12351 0.01745,0.2502 0,0.37343 -0.01379,0.0974 -0.07653,0.183 -0.09258,0.28007 -0.01522,0.0921 0,0.18672 0,0.28007 0,0.18672 0,0.37343 0,0.56015 0,0.37343 0,0.74686 0,1.12029 0,0.34231 0,0.68462 0,1.02693 0,0.12447 0,0.24895 0,0.37343 0,0.0622 0.03398,0.13476 0,0.18671 -0.03768,0.0576 -0.122142,0.0622 -0.18321,0.0934"
           style="fill:none;stroke:#000092;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1" />
        <path
           inkscape:connector-curvature="0"
           id="path1595-9"
           d="m 22.952372,176.84581 c 1.728101,0.27767 1.221578,0.13753 3.230299,0.79399 0.500992,0.16373 1.013924,0.30398 1.488342,0.53456 0.289786,0.14084 0.513668,0.39147 0.782752,0.56923 0.09859,0.0651 0.21148,0.10495 0.317222,0.15742"
           style="fill:none;stroke:#000092;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1"
           inkscape:transform-center-x="0.49691998"
           inkscape:transform-center-y="-0.42172523" />
        <path
           inkscape:connector-curvature="0"
           id="path1597-1"
           d="m 22.582639,176.91979 c -0.987297,0.63788 -2.174925,0.86442 -3.183949,1.45094 -0.340902,0.19816 -0.577526,0.54149 -0.834207,0.84224 -0.479235,0.5615 -0.30419,0.41832 -0.487474,0.87769 -0.07222,0.18102 -0.167352,0.29982 -0.27773,0.46678"
           style="fill:none;stroke:#000092;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1" />
        <path
           style="fill:none;stroke:#000092;stroke-width:0.371282px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1"
           d="m 22.746006,166.38706 c -0.500627,0.58899 -0.414392,0.52079 -1.316228,1.23135 -1.102922,0.86899 -1.36014,1.04415 -2.600223,1.57205 -0.250116,0.10647 -0.507157,0.20219 -0.772421,0.26047 -0.134558,0.0296 -0.275431,0 -0.413142,0 -0.131327,0 -0.262998,0.01 -0.393973,0 -0.391126,-0.0288 -1.855329,-0.21158 -2.120622,-0.23825 -0.431899,-0.0434 -0.865828,-0.0648 -1.297512,-0.11036 -0.230044,-0.0243 -0.39255,-0.0215 -0.39255,-0.30685"
           id="path1676"
           inkscape:connector-curvature="0" />
      </g>
      <text
         xml:space="preserve"
         style="font-style:normal;font-weight:normal;font-size:6.24733px;font-family:sans-serif;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.260305"
         x="126.29302"
         y="199.5779"
         id="text968"
         transform="scale(0.99580954,1.0042081)"><tspan
           sodipodi:role="line"
           id="tspan966"
           x="126.29302"
           y="199.5779"
           style="font-size:6.24733px;stroke-width:0.260305">Framebuffer B</tspan></text>
      <g
         id="g1607-7"
         transform="matrix(0.71736874,0,0,0.71736874,116.26243,54.433956)">
        <path
           inkscape:connector-curvature="0"
           id="path1587-0"
           d="m 14.654395,156.3148 c 0.03086,-0.21784 -0.01811,-0.46387 0.09258,-0.6535 0.210915,-0.36136 0.51464,-0.66709 0.834545,-0.93494 0.814735,-0.68217 1.783373,-1.2107 2.7872,-1.54723 0.410363,-0.13757 0.833709,-0.25283 1.264272,-0.29345 0.285209,-0.0269 0.57532,0.0282 0.854346,0.0935 0.54702,0.12814 0.98993,0.33503 1.296073,0.84022 0.209186,0.34519 0.377663,0.72497 0.462882,1.12028 0.07211,0.3345 0.03218,0.68617 0,1.02693 -0.02984,0.31593 -0.08973,0.63116 -0.185152,0.93358 -0.292505,0.92706 -0.579544,1.46479 -1.296073,2.14722 -0.453148,0.43158 -1.177044,0.74826 -1.758958,0.93358 -0.707129,0.22518 -1.278625,0.25621 -2.036685,0.18671 -0.283337,-0.026 -0.569033,-0.0802 -0.833191,-0.18671 -0.449442,-0.1813 -0.656571,-0.46696 -0.925765,-0.84022 -0.355243,-0.49258 -0.367751,-0.50638 -0.55546,-1.02693 -0.233946,-0.64878 -0.185153,-0.40996 -0.185153,-1.02693 0,-0.12448 -0.02892,-0.25242 0,-0.37343 0.03408,-0.1426 0.123028,-0.26583 0.184542,-0.39873 z"
           style="fill:none;stroke:#000092;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1" />
        <path
           inkscape:connector-curvature="0"
           id="path1589-9"
           d="m 18.010729,160.31885 c 0.03086,0.3674 0.04686,0.73638 0.09258,1.1022 0.06172,0.37344 0.123436,0.74686 0.185155,1.12029 0,0.12448 -0.01364,0.24972 0,0.37343 0.01737,0.15771 0.0752,0.30908 0.09258,0.46679 0.02302,0.20895 0,0.52208 0,0.74686 0,0.59126 0,1.18253 0,1.77379 0,0.18672 -0.01421,0.37398 0,0.56015 0.04567,0.59864 0.141178,0.58116 0.09258,1.12028 0.03266,0.32945 -0.14612,0.61867 -0.185152,0.93358 -0.01532,0.12351 0.01745,0.2502 0,0.37343 -0.01379,0.0974 -0.07653,0.183 -0.09258,0.28007 -0.01522,0.0921 0,0.18672 0,0.28007 0,0.18672 0,0.37343 0,0.56015 0,0.37343 0,0.74686 0,1.12029 0,0.34231 0,0.68462 0,1.02693 0,0.12447 0,0.24895 0,0.37343 0,0.0622 0.03398,0.13476 0,0.18671 -0.03768,0.0576 -0.122142,0.0622 -0.18321,0.0934"
           style="fill:none;stroke:#000092;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1" />
        <path
           inkscape:connector-curvature="0"
           id="path1593-3"
           d="m 18.248815,163.85896 c -1.53536,-0.87272 -2.415761,-1.41768 -4.194631,-2.14689 -1.727333,-0.70808 -2.553291,-0.9287 -4.027558,-1.35306"
           style="fill:none;stroke:#000092;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1" />
        <path
           inkscape:connector-curvature="0"
           id="path1595-6"
           d="m 18.600566,172.04684 c 1.355555,1.11596 0.987512,0.73807 2.396867,2.32373 0.351503,0.39548 0.725028,0.77672 1.02024,1.21652 0.180323,0.26864 0.24895,0.59879 0.392977,0.88885 0.05277,0.10627 0.130517,0.19792 0.195777,0.29688"
           style="fill:none;stroke:#000092;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1" />
        <path
           inkscape:connector-curvature="0"
           id="path1597-0"
           d="m 17.77454,172.36016 c -0.987297,0.63788 -2.174925,0.86442 -3.183949,1.45094 -0.340902,0.19816 -0.577526,0.54149 -0.834207,0.84224 -0.479235,0.5615 -0.30419,0.41832 -0.487474,0.87769 -0.07222,0.18102 -0.167352,0.29982 -0.27773,0.46678"
           style="fill:none;stroke:#000092;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1" />
        <path
           inkscape:connector-curvature="0"
           id="path1599-6"
           d="m 19.11498,163.49801 c 1.707836,-1.30942 1.011331,-0.99818 2.708093,-1.62038 0.275136,-0.10089 0.546137,-0.22217 0.833214,-0.28007 0.181557,-0.0366 0.370306,0 0.555458,0 0.277731,0 0.555461,0 0.833191,0"
           style="fill:none;stroke:#000092;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1" />
      </g>
      <text
         xml:space="preserve"
         style="font-style:normal;font-weight:normal;font-size:4.51196px;font-family:sans-serif;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.260305"
         x="132.1199"
         y="204.49637"
         id="text968-2"
         transform="scale(0.99580954,1.0042081)"><tspan
           sodipodi:role="line"
           id="tspan966-6"
           x="132.1199"
           y="204.49637"
           style="font-style:italic;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:4.51196px;font-family:sans-serif;-inkscape-font-specification:'sans-serif Italic';stroke-width:0.260305">(being drawn)</tspan></text>
      <text
         xml:space="preserve"
         style="font-style:normal;font-weight:normal;font-size:4.51196px;font-family:sans-serif;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.260305"
         x="128.39224"
         y="157.04173"
         id="text968-2-1"
         transform="scale(0.99580954,1.0042081)"><tspan
           sodipodi:role="line"
           id="tspan966-6-8"
           x="128.39224"
           y="157.04173"
           style="font-style:italic;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:4.51196px;font-family:sans-serif;-inkscape-font-specification:'sans-serif Italic';stroke-width:0.260305">(currently active)</tspan></text>
      <text
         xml:space="preserve"
         style="font-style:normal;font-weight:normal;font-size:6.24733px;font-family:sans-serif;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.260305"
         x="216.48663"
         y="183.85321"
         id="text137-2"
         transform="scale(0.99580954,1.0042081)"><tspan
           sodipodi:role="line"
           id="tspan135-0"
           x="216.48663"
           y="183.85321"
           style="font-size:5.55318px;stroke-width:0.260305">User Program</tspan></text>
      <rect
         style="fill:none;stroke:#000000;stroke-width:0.7809162;stroke-miterlimit:4;stroke-dasharray:2.34274859,2.34274859;stroke-dashoffset:0.7809162;stroke-opacity:1"
         id="rect3467"
         width="66.300957"
         height="65.957413"
         x="203.0231"
         y="187.55788" />
      <rect
         style="fill:none;stroke:#000000;stroke-width:0.7809162;stroke-miterlimit:4;stroke-dasharray:2.34274859,2.34274859;stroke-dashoffset:0.7809162;stroke-opacity:1"
         id="rect3467-7"
         width="66.422058"
         height="51.304924"
         x="202.96255"
         y="119.99245" />
      <text
         id="text4586-31"
         y="157.37607"
         x="205.19287"
         style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:5.55317px;line-height:0.75;font-family:Cursor;-inkscape-font-specification:Cursor;letter-spacing:0px;word-spacing:0px;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.00983832;stroke-miterlimit:4;stroke-dasharray:0.00983832, 0.029515;stroke-dashoffset:0;stroke-opacity:1"
         xml:space="preserve"><tspan
           style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:4.51196px;font-family:'Latin Modern Mono';-inkscape-font-specification:'Latin Modern Mono';stroke:none;stroke-width:0.00983832;stroke-miterlimit:4;stroke-dasharray:0.00983832, 0.029515;stroke-dashoffset:0;stroke-opacity:1"
           y="157.37607"
           x="205.19287"
           sodipodi:role="line"
           id="tspan3435-6">DrawLine()</tspan><tspan
           style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:4.51196px;font-family:'Latin Modern Mono';-inkscape-font-specification:'Latin Modern Mono';stroke:none;stroke-width:0.00983832;stroke-miterlimit:4;stroke-dasharray:0.00983832, 0.029515;stroke-dashoffset:0;stroke-opacity:1"
           y="162.49635"
           x="205.19287"
           sodipodi:role="line"
           id="tspan3548">FillRect()</tspan><tspan
           style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:4.51196px;font-family:'Latin Modern Mono';-inkscape-font-specification:'Latin Modern Mono';stroke:none;stroke-width:0.00983832;stroke-miterlimit:4;stroke-dasharray:0.00983832, 0.029515;stroke-dashoffset:0;stroke-opacity:1"
           y="167.61664"
           x="205.19287"
           sodipodi:role="line"
           id="tspan3550">DrawCircle()</tspan></text>
      <rect
         y="132.40334"
         x="118.98624"
         height="8.5012817"
         width="56.818985"
         id="rect4555-7"
         style="fill:none;fill-opacity:1;fill-rule:evenodd;stroke:#730000;stroke-width:0.983831;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1" />
      <text
         xml:space="preserve"
         style="font-style:normal;font-weight:normal;font-size:3.47073px;font-family:sans-serif;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.260305"
         x="129.04559"
         y="137.03883"
         id="text956-4"
         transform="scale(0.99580954,1.0042081)"><tspan
           sodipodi:role="line"
           id="tspan954-0"
           x="129.04559"
           y="137.03883"
           style="font-size:3.47073px;stroke-width:0.260305">operation parameters</tspan></text>
      <path
         style="fill:none;stroke:#000000;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1;marker-end:url(#Arrow2Lend-8)"
         d="m 262.47168,186.76978 c 0,-9.55283 0.005,-9.55603 0.005,-14.24074"
         id="path1014-8"
         inkscape:connector-curvature="0"
         sodipodi:nodetypes="cc" />
      <path
         style="fill:none;stroke:#000000;stroke-width:0.260305px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1;marker-end:url(#Arrow2Lend-8-2)"
         d="m 210.5257,186.76978 c 0,-9.55283 0.005,-9.55603 0.005,-14.24074"
         id="path1014-8-0"
         inkscape:connector-curvature="0"
         sodipodi:nodetypes="cc" />
      <text
         id="text4586-31-6"
         y="195.36339"
         x="206.00818"
         style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:6.94148px;line-height:0.75;font-family:Cursor;-inkscape-font-specification:Cursor;letter-spacing:0px;word-spacing:0px;fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.00983832;stroke-miterlimit:4;stroke-dasharray:0.00983832, 0.029515;stroke-dashoffset:0;stroke-opacity:1"
         xml:space="preserve"><tspan
           style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:6.24733px;font-family:'Latin Modern Mono';-inkscape-font-specification:'Latin Modern Mono';stroke:none;stroke-width:0.00983832;stroke-miterlimit:4;stroke-dasharray:0.00983832, 0.029515;stroke-dashoffset:0;stroke-opacity:1"
           y="195.36339"
           x="206.00818"
           sodipodi:role="line"
           id="tspan3550-6">drawBackground();</tspan><tspan
           style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:6.24733px;font-family:'Latin Modern Mono';-inkscape-font-specification:'Latin Modern Mono';stroke:none;stroke-width:0.00983832;stroke-miterlimit:4;stroke-dasharray:0.00983832, 0.029515;stroke-dashoffset:0;stroke-opacity:1"
           y="202.16037"
           x="206.00818"
           sodipodi:role="line"
           id="tspan7999">while(walk)</tspan><tspan
           style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:6.24733px;font-family:'Latin Modern Mono';-inkscape-font-specification:'Latin Modern Mono';stroke:none;stroke-width:0.00983832;stroke-miterlimit:4;stroke-dasharray:0.00983832, 0.029515;stroke-dashoffset:0;stroke-opacity:1"
           y="208.95735"
           x="206.00818"
           sodipodi:role="line"
           id="tspan8001">    animateMan();</tspan><tspan
           style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:6.24733px;font-family:'Latin Modern Mono';-inkscape-font-specification:'Latin Modern Mono';stroke:none;stroke-width:0.00983832;stroke-miterlimit:4;stroke-dasharray:0.00983832, 0.029515;stroke-dashoffset:0;stroke-opacity:1"
           y="215.75433"
           x="206.00818"
           sodipodi:role="line"
           id="tspan8003">...</tspan><tspan
           style="font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:6.24733px;font-family:'Latin Modern Mono';-inkscape-font-specification:'Latin Modern Mono';stroke:none;stroke-width:0.00983832;stroke-miterlimit:4;stroke-dasharray:0.00983832, 0.029515;stroke-dashoffset:0;stroke-opacity:1"
           y="220.96045"
           x="206.00818"
           sodipodi:role="line"
           id="tspan7997" /></text>
    </g>
    <g
       id="g2933"
       transform="translate(-6.2589386,10.663127)">
      <path
         sodipodi:nodetypes="ccc"
         inkscape:connector-curvature="0"
         id="path1014-9"
         d="m 201.33772,126.84083 c -13.07281,0 -13.16904,-0.063 -13.16904,5.61196 l 0,116.2184"
         style="fill:none;stroke:#000000;stroke-width:0.256854px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1"
         transform="matrix(1.0207115,0,0,1.012175,10.579452,-3.1117996)" />
      <path
         sodipodi:nodetypes="cc"
         inkscape:connector-curvature="0"
         id="path172-7"
         d="m 188.17225,200.1374 c 0,3.035 0.0278,3.02118 -8.3927,3.02118"
         style="fill:none;stroke:#000000;stroke-width:0.260714;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1;marker-end:url(#marker1266-3)"
         transform="matrix(1.0207115,0,0,1.012175,10.579452,-3.1117996)" />
      <path
         sodipodi:nodetypes="cc"
         inkscape:connector-curvature="0"
         id="path172-5"
         d="m 188.17664,151.89449 c 0,3.035 0.0278,3.02118 -8.39269,3.02118"
         style="fill:none;stroke:#000000;stroke-width:0.260714;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1;marker-end:url(#marker1266-5)"
         transform="matrix(1.0207115,0,0,1.012175,10.579452,-3.1117996)" />
      <path
         sodipodi:nodetypes="cc"
         inkscape:connector-curvature="0"
         id="path172"
         d="m 202.64162,248.45709 c 0,3.07195 0.0284,3.05796 -8.56652,3.05796"
         style="fill:none;stroke:#000000;stroke-width:0.265;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1;marker-end:url(#marker1266)" />
    </g>
    <g
       transform="matrix(-1,0,0,1,310.65227,10.663127)"
       id="g2933-0">
      <path
         sodipodi:nodetypes="ccsc"
         inkscape:connector-curvature="0"
         id="path1014-9-9"
         d="M 188.16868,248.67119 V 127.01468 c 0,-15.496 0.0422,-15.62948 5.01668,-15.62948 6.79558,0 28.0581,0.0418 59.16161,0.0418"
         style="fill:none;stroke:#000000;stroke-width:0.256854px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1"
         transform="matrix(1.0207115,0,0,1.012175,10.579452,-3.1117996)" />
      <path
         sodipodi:nodetypes="cc"
         inkscape:connector-curvature="0"
         id="path172-7-3"
         d="m 188.17225,200.1374 c 0,3.035 0.0307,3.02118 -6.22033,3.02118"
         style="fill:none;stroke:#000000;stroke-width:0.260714;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1;marker-end:url(#Arrow2Lstart)"
         transform="matrix(1.0207115,0,0,1.012175,10.579452,-3.1117996)" />
      <path
         sodipodi:nodetypes="cc"
         inkscape:connector-curvature="0"
         id="path172-5-6"
         d="m 188.17664,151.89449 c 0,3.035 0.04,3.02118 -6.35377,3.02118"
         style="fill:none;stroke:#000000;stroke-width:0.260714;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1;marker-end:url(#marker2693)"
         transform="matrix(1.0207115,0,0,1.012175,10.579452,-3.1117996)" />
      <path
         sodipodi:nodetypes="cc"
         inkscape:connector-curvature="0"
         id="path172-0"
         d="m 202.64162,248.45709 c 0,3.07195 -0.11214,3.05796 -6.75753,3.05796"
         style="fill:none;stroke:#000000;stroke-width:0.265;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1;marker-end:url(#marker2683)" />
      <path
         sodipodi:nodetypes="cc"
         inkscape:connector-curvature="0"
         id="path172-5-6-2"
         d="m 196.11394,112.39886 c 6.71073,0 6.73406,-0.0101 7.43753,-1.38556"
         style="fill:none;stroke:#000000;stroke-width:0.265;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1;marker-start:url(#marker2117)" />
    </g>
    <path
       style="fill:none;stroke:#000000;stroke-width:0.79375;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:2.38125,2.38125;stroke-dashoffset:0.79375"
       d="m 211.69655,150.79328 h 67.7339"
       id="path3494"
       inkscape:connector-curvature="0" />
    <path
       style="fill:none;stroke:#000000;stroke-width:0.265;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1;marker-start:url(#marker1602)"
       d="m 185.67631,135.86353 c 7.06829,0 7.19013,0.0325 7.19013,-4.51347 0,-0.63824 -0.024,-6.26995 -0.024,-7.85898 0,-1.1412 -0.0483,-1.66776 3.09567,-1.66776 2.52666,0 8.12545,-0.0704 13.96142,-0.0704"
       id="path1014-4"
       inkscape:connector-curvature="0"
       sodipodi:nodetypes="cccsc" />
  </g>
</svg>

jobs: #-------------------------------------------------
#
# Project created by QtCreator 2018-09-10T15:30:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VP-Display
TEMPLATE = app
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    vpdisplayserver.cpp

HEADERS += \
        mainwindow.h \
    vpdisplayserver.h \
    framebuffer.h

FORMS += \
        mainwindow.ui
  build: #pragma once
#include <inttypes.h>
#include <cassert>
#include <cstring>

#define SHMKEY 1338

struct Framebuffer {
        static constexpr uint16_t screenWidth = 800;
        static constexpr uint16_t screenHeight = 600;

        typedef uint16_t Color;

        struct Point {
                uint32_t x;
                uint32_t y;
                inline Point() : x(0), y(0){};
                inline Point(uint32_t x, uint32_t y) : x(x), y(y){};
        };

        struct PointF {
                float x;
                float y;
                inline PointF() : x(0), y(0){};
                inline PointF(float x, float y) : x(x), y(y){};
                inline PointF(Point p) : x(p.x), y(p.y){};
        };

        struct Frame {
                Color raw[screenHeight][screenWidth];  // Notice: Screen is on side
        };

        enum class Type : uint8_t { foreground, background };
        uint8_t activeFrame;
        enum class Command : uint8_t {
                none = 0,
                clearAll,
                fillFrame,
                applyFrame,
                drawLine,
        } volatile command;
        union Parameter {
                struct {
                        //fillframe
                        Type frame;
                        Color color;
                } fill;
                struct {
                        //drawLine
                        Type frame;
                        PointF from;
                        PointF to;
                        Color color;
                } line;
                inline Parameter(){};
        } parameter;
        Frame frames[2];
        Frame background;

        Framebuffer() : activeFrame(0), command(Command::none){};

        Frame& getActiveFrame() {
                return frames[activeFrame % 2];
        }
        Frame& getInactiveFrame() {
                return frames[(activeFrame + 1) % 2];
        }
        Frame& getBackground() {
                return background;
        }
        Frame& getFrame(Type type) {
                if (type == Type::foreground)
                        return getInactiveFrame();
                else if (type == Type::background)
                        return getBackground();

                assert(false && "Get invalid frame type");
                return background;
        }
};


inline Framebuffer::PointF operator+(const Framebuffer::PointF l, Framebuffer::PointF const r) {
        return Framebuffer::PointF(l.x + r.x, l.y + r.y);
}

    runs-on: #include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
        QApplication a(argc, argv);
        VPDisplay w;
        w.show();

        return a.exec();
}

    steps: #include "mainwindow.h"
#include <qpainter.h>
#include <cassert>
#include "framebuffer.h"
#include "ui_mainwindow.h"

VPDisplay::VPDisplay(QWidget* mparent) : QWidget(mparent) {
        framebuffer = server.createSM();
        frame = new QImage(Framebuffer::screenWidth, Framebuffer::screenHeight,
                           QImage::Format_RGB444);  // two bytes per pixel
        resize(Framebuffer::screenWidth, Framebuffer::screenHeight);
        setFixedSize(size());
        server.startListening(std::bind(&VPDisplay::notifyChange, this, std::placeholders::_1));
}

VPDisplay::~VPDisplay() {
        delete frame;
}

void VPDisplay::drawMainPage(QImage* mem) {
        Framebuffer::Frame activeFrame = framebuffer->getActiveFrame();
        Framebuffer::Frame background = framebuffer->getBackground();
        for (int row = 0; row < mem->height(); row++) {
                uint16_t* line = reinterpret_cast<uint16_t*>(mem->scanLine(row));  // Two bytes per pixel
                for (int x = 0; x < mem->width(); x++) {
                        line[x] = activeFrame.raw[row][x] == 0 ? background.raw[row][x] : activeFrame.raw[row][x];
                }
        }
}

void VPDisplay::paintEvent(QPaintEvent*) {
        QPainter painter(this);

        // painter.scale(size_factor, size_factor);

        // Draw Header
        // QPainter mempaint(&memory);

        drawMainPage(frame);
        painter.drawImage(QPoint(0, 0), *frame);
        painter.end();
}

void VPDisplay::notifyChange(bool success) {
        assert(success);
        update();
}
    - uses: #pragma once
#include <QMainWindow>
#include <cassert>
#include "vpdisplayserver.h"

namespace Ui {
class VPDisplay;
}

class VPDisplay : public QWidget {
        Q_OBJECT
        Framebuffer* framebuffer;
        QImage* frame;
        VPDisplayserver server;

   public:
        VPDisplay(QWidget* mparent = 0);
        ~VPDisplay();
        void paintEvent(QPaintEvent*);
        // void keyPressEvent(QKeyEvent *e);
        void drawMainPage(QImage* mem);

        void notifyChange(bool success);
};
    - name: <ui version="4.0">
 <class>MainWindow</class>
 <widget class="QMainWindow" name="MainWindow" >
  <property name="geometry" >
   <rect>
    <x>0</x>
    <y>0</y>
    <width>400</width>
    <height>300</height>
   </rect>
  </property>
  <property name="windowTitle" >
   <string>MainWindow</string>
  </property>
  <widget class="QMenuBar" name="menuBar" />
  <widget class="QToolBar" name="mainToolBar" />
  <widget class="QWidget" name="centralWidget" />
  <widget class="QStatusBar" name="statusBar" />
 </widget>
 <layoutDefault spacing="6" margin="11" />
 <pixmapfunction></pixmapfunction>
 <resources/>
 <connections/>
</ui>
      run: #include "vpdisplayserver.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <iostream>

VPDisplayserver::VPDisplayserver(unsigned int sharedMemoryKey) : mSharedMemoryKey(sharedMemoryKey), stop(false) {}

VPDisplayserver::~VPDisplayserver() {
        stop.store(true);
        active_watch.join();
}

Framebuffer* VPDisplayserver::createSM() {
        int shmid;
        // TODO: Dont create, but check if exists
        if ((shmid = shmget(mSharedMemoryKey, sizeof(Framebuffer), 0666)) < 0) {
                perror("shmget");
                exit(1);
        }
        std::cout << "SHMID: " << shmid << std::endl;
        framebuffer = reinterpret_cast<Framebuffer*>(shmat(shmid, nullptr, 0));
        if (framebuffer == (Framebuffer*)-1) {
                perror("shmat");
                exit(1);
        }
        return framebuffer;
}

void VPDisplayserver::startListening(std::function<void(bool)> notifyChange) {
        // TODO: While loop around buffer changer
        active_watch = std::thread([=]() {
                uint8_t lastFrame = -1;
                while (!stop.load()) {
                        if (framebuffer->activeFrame != lastFrame) {
                                notifyChange(true);
                                lastFrame = framebuffer->activeFrame;
                        }
                        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
        });
        notifyChange(true);
}
    - name: #pragma once
#include <atomic>
#include <functional>
#include <thread>
#include "framebuffer.h"

class VPDisplayserver {
        unsigned int mSharedMemoryKey;
        Framebuffer* framebuffer;
        std::atomic<bool> stop;
        std::thread active_watch;

   public:
        VPDisplayserver(unsigned int sharedMemoryKey = 1338);
        ~VPDisplayserver();
        Framebuffer* createSM();
        void startListening(std::function<void(bool)> notifyChange);
};
      run: cargo test --verbose

