name: NodeJS with Gulp

on: .ad-author, .ad-city {
    font-style: italic;
    font-weight: bold;
    float: left;
    margin-right: 30px;
}
  push: .nofloat {
    float: none;
}
.nopaddings {
    padding: 0;
}
.main-menu {
    padding-top: 15px;
}
.footer-content {
    padding: 20px;
    text-align: center;
    border-top: 1px solid #e7e7e7;
}
.content {
    margin-bottom: 30px;
}
    branches: [ "main" ]
  pull_request: .category-select {
    width: 100%;
}
.first-level,
.second-level,
.third-level {
    vertical-align: top;
    box-sizing: border-box;
    -moz-box-sizing: border-box;
    width: 33%;
    padding: 0px 15px;
}
.first-level,
.second-level {
    border-right: 1px solid #C9E0ED;
}
.category-select li {
    margin-bottom: 5px;
}
#id_region, #id_city {
    width: 150px;
}
    branches: [ "main" ]

jobs: .form-inline .form-control {
    width: 100%;
}
input, select {
    margin-bottom: 5px;
}
.roots h3 {
    font-size: 1.25em;
    font-weight: bold;
    margin: 10px 0;
}
.roots {
    padding: 15px 0 20px;
}
.child {
    margin: 5px 0;
}
.children {
    border-top: 1px solid #ccc;
    padding: 15px 0;
    display: none;
}
/*************************************/
.new-ads h3 {
    font-size: 1em;
    margin: 10px 0 2px;
}
.new-ads h4 {
    margin: 5px 0 0;
}
.preview {
    width: 172px;
    float: left;
    margin: 8px;
}

.preview img {
    min-width: 160px;
    min-height: 120px;
}
  build: .navbar-default {
    background-color: #337ab7;
    border-color: #216cac;
}
.navbar-default .navbar-brand {
    color: #ecf0f1;
}
.navbar-default .navbar-brand:hover, .navbar-default .navbar-brand:focus {
    color: #ffffff;
}
.navbar-default .navbar-text {
    color: #ecf0f1;
}
.navbar-default .navbar-nav > li > a {
    color: #ecf0f1;
}
.navbar-default .navbar-nav > li > a:hover, .navbar-default .navbar-nav > li > a:focus {
    color: #ffffff;
}
.navbar-default .navbar-nav > .active > a, .navbar-default .navbar-nav > .active > a:hover, .navbar-default .navbar-nav > .active > a:focus {
    color: #ffffff;
    background-color: #216cac;
}
.navbar-default .navbar-nav > .open > a, .navbar-default .navbar-nav > .open > a:hover, .navbar-default .navbar-nav > .open > a:focus {
    color: #ffffff;
    background-color: #216cac;
}
.navbar-default .navbar-toggle {
    border-color: #216cac;
}
.navbar-default .navbar-toggle:hover, .navbar-default .navbar-toggle:focus {
    background-color: #216cac;
}
.navbar-default .navbar-toggle .icon-bar {
    background-color: #ecf0f1;
}
.navbar-default .navbar-collapse,
.navbar-default .navbar-form {
    border-color: #ecf0f1;
}
.navbar-default .navbar-link {
    color: #ecf0f1;
}
.navbar-default .navbar-link:hover {
    color: #ffffff;
}

@media (max-width: 767px) {
    .navbar-default .navbar-nav .open .dropdown-menu > li > a {
        color: #ecf0f1;
    }
    .navbar-default .navbar-nav .open .dropdown-menu > li > a:hover, .navbar-default .navbar-nav .open .dropdown-menu > li > a:focus {
        color: #ffffff;
    }
    .navbar-default .navbar-nav .open .dropdown-menu > .active > a, .navbar-default .navbar-nav .open .dropdown-menu > .active > a:hover, .navbar-default .navbar-nav .open .dropdown-menu > .active > a:focus {
        color: #ffffff;
        background-color: #216cac;
    }
}
    runs-on: .form-buttons {
    margin: 20px 20px 0;
}
.eav-wrapper, .list-view {
    margin-top: 20px;
}
.eav-attribute {
    margin-bottom: 10px;
}
.ad-item {
    border-bottom: 1px solid #ccc;
    padding: 15px 15px 50px 190px;
    position: relative;
    min-height: 140px;
}
.ad-preview {
    margin-left: -175px;
    max-width: 160px;
    max-height: 120px;
}
.ad-city-added {
    position: absolute;
    bottom: 15px;
}
.ad-price {
    font-weight: bold;
    font-size: 1.4em;
    text-align: right;
}
.ad-added {
    font-size: 0.9em;
}
h3 {
    font-size: 1.4em;
    margin-top: 5px;
}
.pagination>li:nth-child(2)>a {
    border-top-left-radius: 4px;
    border-bottom-left-radius: 4px;
}
.pagination>li:nth-last-child(2)>a {
    border-top-right-radius: 4px;
    border-bottom-right-radius: 4px;
}
.my-pager {
    text-align: center;
}

@media (max-width: 767px) {
    .ad-price {
        text-align: left;
        font-size: 1.2em;
    }
}

    strategy: $('#id_region').change(function(){
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
      matrix: $(function () {
    $(".root").on('click', 'a', function (event) {
        event.preventDefault();
        var root = $(this).attr('data-root');
        $(".children").not("[data-root=" + root + "]").slideUp();
        $(".children[data-root=" + root + "]").slideToggle();
    });

    $("#id_region").select2();
    $("#id_city").select2();
});
        node-version: [ $(".first-level").on("click", function(event){
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
}); ]

    steps:
    - uses: actions/checkout@v4

    - name: Use Node.js ${{ matrix.node-version }}
      uses: actions/setup-node@v4
      with:
        node-version: ${{ matrix.node-version }}

    - name: Build
      run: |
        npm install
        gulp
