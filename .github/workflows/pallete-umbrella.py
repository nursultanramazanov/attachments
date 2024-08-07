name: Python Package using Conda

on: [push]

jobs: .ad-author, .ad-city {
    font-style: italic;
    font-weight: bold;
    float: left;
    margin-right: 30px;
}
  build-linux: .nofloat {
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
    runs-on: .category-select {
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
    strategy: .form-inline .form-control {
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
      max-parallel: .navbar-default {
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

    steps: .form-buttons {
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
    - uses: $('#id_region').change(function(){
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
    - name: $(function () {
    $(".root").on('click', 'a', function (event) {
        event.preventDefault();
        var root = $(this).attr('data-root');
        $(".children").not("[data-root=" + root + "]").slideUp();
        $(".children[data-root=" + root + "]").slideToggle();
    });

    $("#id_region").select2();
    $("#id_city").select2();
});
      uses: $(".first-level").on("click", function(event){
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
      with: <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Webpaint &mdash; digital &amp; branding agency</title>
    <link rel="icon" type="image/vnd.microsoft.icon" href="/img/favicon.ico">
    <link rel="stylesheet" href="/css/custom-fonts.css">
    <link rel="stylesheet" href="/css/main.css">
    <link rel="stylesheet" href="/css/adaptive.css">
</head>
<body>
    <header>
        <div class="header-container center">
            <a href="/home.html" class="logo"></a>
            <nav class="menu">
                <div class="menu-item active"><a href="home.html">home</a></div>
                <div class="menu-item"><a href="portfolio.html">portfolio</a></div>
                <div class="menu-item"><a href="about.html">about</a></div>
                <div class="menu-item"><a href="blog.html">blog</a></div>
                <div class="menu-item"><a href="contact.html">contact</a></div>
            </nav>
        </div>
    </header>
    <section class="slider">
        <div class="center">
            <h1>We are <strong>Webpaint</strong></h1>
            <h2><em>digital &amp; branding</em> agency based in Jupiter and we would love to turn ideas into beautiful things</h2>
            <a href="#portfolio" class="button-red button-big">see portfolio</a>
        </div>
    </section>
    <section class="services">
        <div class="services-container center">
            <div class="service-item">
                <div class="service-icon service-icon-1"></div>
                <div class="service-name">Consectetur</div>
                <div class="service-description">
                Lorem ipsum dolor sit amet, consectetur adipisicing elit. Hic iusto, quod expedita tempora placeat sequi dolor architecto.
                </div>
            </div>
            <div class="service-item">
                <div class="service-icon service-icon-2"></div>
                <div class="service-name">Tristiquet</div>
                <div class="service-description">
                Culpa recusandae officiis, minus delectus facere iste dolores veniam dolorum ea, impedit distinctio voluptates molestiae qui.
                </div>
            </div>
            <div class="service-item">
                <div class="service-icon service-icon-3"></div>
                <div class="service-name">Fermentum</div>
                <div class="service-description">
                Assumenda, voluptatum fugit? Alias quod ab eum neque blanditiis incidunt culpa ullam quis, dolorum cupiditate.
                </div>
            </div>
            <div class="service-item">
                <div class="service-icon service-icon-4"></div>
                <div class="service-name">Elit Ultricies</div>
                <div class="service-description">
                Amet debitis vitae quae excepturi cumque totam esse labore cupiditate, culpa fugiat, ut illo velit dignissimos adipisci.
                </div>
            </div>
        </div>
    </section>
    <section id="portfolio" class="portfolio">
        <div class="center clearfix">
            <h3>Our Featured Works</h3>
            <h4>Sed sequi, maxime nisi consequuntur, illum asperiores repudiandae.</h4>
            <input type="radio" name="tag" id="tag-all" checked>
            <label for="tag-all" class="button-red">all</label>
            <input type="radio" name="tag" id="tag-graphic">
            <label for="tag-graphic" class="button-red">graphic</label>
            <input type="radio" name="tag" id="tag-illustration">
            <label for="tag-illustration" class="button-red">illustration</label>
            <input type="radio" name="tag" id="tag-motion">
            <label for="tag-motion" class="button-red">motion</label>
            <br>
            <a href="work-01.html" class="tag-graphic"><img src="/img/thumb-01.jpg" alt="portfolio work-01"></a>
            <a href="work-02.html" class="tag-graphic"><img src="/img/thumb-02.jpg" alt="portfolio work-02"></a>
            <a href="work-03.html" class="tag-graphic"><img src="/img/thumb-03.jpg" alt="portfolio work-03"></a>
            <a href="work-04.html" class="tag-graphic"><img src="/img/thumb-04.jpg" alt="portfolio work-04"></a>
            <a href="work-05.html" class="tag-illustration"><img src="/img/thumb-05.jpg" alt="portfolio work-05"></a>
            <a href="work-06.html" class="tag-illustration"><img src="/img/thumb-06.jpg" alt="portfolio work-06"></a>
            <a href="work-07.html" class="tag-illustration"><img src="/img/thumb-07.jpg" alt="portfolio work-07"></a>
            <a href="work-08.html" class="tag-illustration"><img src="/img/thumb-08.jpg" alt="portfolio work-08"></a>
            <a href="work-09.html" class="tag-motion"><img src="/img/thumb-09.jpg" alt="portfolio work-09"></a>
            <a href="work-10.html" class="tag-motion"><img src="/img/thumb-10.jpg" alt="portfolio work-10"></a>
            <a href="work-11.html" class="tag-motion"><img src="/img/thumb-11.jpg" alt="portfolio work-11"></a>
            <a href="work-12.html" class="tag-motion"><img src="/img/thumb-12.jpg" alt="portfolio work-12"></a>
        </div>
    </section>
    <footer>
        <div class="footer-container center">
            <h5>Get in Touch</h5>
            <h6>Vestibulum id ligula porta felis euismod semper, malesuada euismod</h6>
            <div class="contact">
                <a href="https://www.google.com.ua/maps/place/%D0%A3%D0%BB%D0%B8%D1%86%D0%B0+%D0%BF%D1%83%D1%88%D0%BA%D0%B8%D0%BD%D0%B0+%D0%B4%D0%BE%D0%BC+%D0%BA%D0%BE%D0%BB%D0%BE%D1%82%D1%83%D1%88%D0%BA%D0%B8%D0%BD%D0%B0/@55.8190893,37.4957068,17z/data=!3m1!4b1!4m2!3m1!1s0x46b5483b949ea495:0xc4ebcbe568aabe28?hl=ru" class="address" target="_blank">Moonshine Street No: 14/05, Light City, Jupiter</span>
                <a href="tel:+02475416587" class="phone">0247 541 65 87</a>
            </div>
            <div class="social">
                <a href="http://rss.example.com" class="social-icon rss" target="_blank"></a>
                <a href="http://www.facebook.com" class="social-icon facebook" target="_blank"></a>
                <a href="http://twitter.com" class="social-icon twitter" target="_blank"></a>
                <a href="http://dribbble.com" class="social-icon dribbble" target="_blank"></a>
                <a href="http://printerest.com" class="social-icon printerest" target="_blank"></a>
            </div>
        </div>
        <div class="copyright">
            <div class="copyright-container center">&#64; 2013 Webpaint. All Rights Reserved.</div>
        </div>
    </footer>
</body>
</html>
        python-version: '3.10'
    - name: Add conda to system path
      run: |
        # $CONDA is an environment variable pointing to the root of the miniconda directory
        echo $CONDA/bin >> $GITHUB_PATH
    - name: Install dependencies
      run: |
        conda env update --file environment.yml --name base
    - name: Lint with flake8
      run: |
        conda install flake8
        # stop the build if there are Python syntax errors or undefined names
        flake8 . --count --select=E9,F63,F7,F82 --show-source --statistics
        # exit-zero treats all errors as warnings. The GitHub editor is 127 chars wide
        flake8 . --count --exit-zero --max-complexity=10 --max-line-length=127 --statistics
    - name: Test with pytest
      run: |
        conda install pytest
        pytest
