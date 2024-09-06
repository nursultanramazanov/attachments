# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: 
#include "MoveCommand.h"

static const std::string _name("move");
static const std::string _description =
    "Arguments: {column_index}{row_index}\n"
    "Example: " +
    _name + " e4\n"
            "Description: Moves the selected piece to the specified square if legal.";

MoveCommand::MoveCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result MoveCommand::apply(const std::vector<std::string> &params)
{
    if (params.size() != 1 || params[0].length() != 2)
        return {true, false};

    int row = params[0][1] - '1';
    int column = params[0][0] - 'a';

    Chess &chess = this->get_chess();
    bool error = chess.move({row, column});
    return {error, false};
}
  push: #pragma once

#include "../Command.h"

// Moves the selected piece to a new square.
// Params:
//      1. string of format {char}{int} representing a
//          field of the chessboard.
class MoveCommand : public Command
{
public:
    MoveCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};
    branches: [ "main" ]
  pull_request: 
#include "QuitCommand.h"

static const std::string _name = "quit";
static const std::string _description =
    "Arguments: [None]\n"
    "Description: Quits the game.";

QuitCommand::QuitCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result QuitCommand::apply(const std::vector<std::string> &params)
{
    bool error = !params.empty();
    bool quit = true;
    return {error, quit};
}
    branches: [ "main" ]
  schedule: #pragma once

#include "../Command.h"

// Sends a quit signal to the session.
// No params.
class QuitCommand : public Command
{
public:
    QuitCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};
    - cron: 
#include "SelectCommand.h"

static const std::string _name("select");
static const std::string _description =
    "Arguments: {column_index}{row_index}\n"
    "Example: " +
    _name + " e2\n"
            "Description: Selects the specified square.";

SelectCommand::SelectCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result SelectCommand::apply(const std::vector<std::string> &params)
{
    if (params.size() != 1 || params[0].length() != 2)
        return {true, false};

    int row = params[0][1] - '1';
    int column = params[0][0] - 'a';

    Chess &chess = this->get_chess();
    bool error = chess.select({row, column});
    return {error, false};
}

env: #pragma once

#include "../Command.h"

// Selects a square.
// Params:
//     1. row
//     2. column
class SelectCommand : public Command
{
public:
    SelectCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};
  # Path to the CMake build directory.
  build: 
#include "Command.h"

Command::Command(Chess &chess, std::string name, std::string description)
    : chess(chess), name(name), description(description) {}

const std::string &Command::get_name() const
{
    return this->name;
}

const std::string &Command::get_description() const
{
    return this->description;
}

Chess &Command::get_chess()
{
    return this->chess;
}

permissions: #pragma once

#include <string>
#include <vector>

#include "Result.h"

#include "../model/Chess.h"

class Command
{
public:
    Command(Chess &chess, std::string name, std::string description);
    virtual ~Command() = default;

    const std::string &get_name() const;
    const std::string &get_description() const;

    virtual Result apply(const std::vector<std::string> &params) = 0;

protected:
    Chess &get_chess();

private:
    Chess &chess;
    std::string name;
    std::string description;
};
  contents: #pragma once

struct Result
{
    bool error;
    bool quit;
};

jobs: 
  analyze: <input type="hidden" name="Ad[category_id]" value="{{model.category_id}}">
    permissions: <div class="row">
    {{ form.labelEx(model, 'category_id') }}
    {{ form.dropDownList(model, 'category_id', model.getCategoryList(App.request.getQuery('id')), {'empty':'выбрать'}) }}
    {{ form.error(model, 'category_id') }}
</div>
      contents: {% extends 'views/layouts/main_old.twig' %}

{% block auxres %}
    {{ Yii.void(App.clientScript.registerCoreScript('jquery')) }}
    {{ Yii.void(App.clientScript.registerScriptFile(
        App.request.baseUrl~'/js/select2-3.5.4/select2.js')) }}
    {{ Yii.void(App.clientScript.registerCssFile(
        App.request.baseUrl~'/js/select2-3.5.4/select2.css')) }}
{% endblock auxres %}

{% block content %}

{{ Yii.void(App.clientScript.registerScriptFile(
    App.request.baseUrl~'/js/citiesDropDown.js', constant('CClientScript::POS_END'))) }}
<div class="form">
{% set form = this.beginWidget('CActiveForm', {
    'htmlOptions': {'enctype':'multipart/form-data'}
}) %}
{{ form.errorSummary(model) }}
{% if model.getCategoryList(App.request.getQuery('id')) %}
    {% include 'views/ad/_subcategory.twig' %}
{% else %}
    {% include 'views/ad/_inputhidden.twig' %}
{% endif %}
<div class="row">
    {{ form.labelEx(model, 'title') }}
    {{ form.textField(model, 'title') }}
    {{ form.error(model, 'title') }}
</div>
<div class="row">
    {{ form.labelEx(model, 'description') }}
    {{ form.textArea(model, 'description') }}
    {{ form.error(model, 'description') }}
</div>
{% for key in model.eavAttributes|keys %}
    {% if model.getEavVariants(key) %} 
        {% include 'views/eav/dropDown.twig' %}
    {% else %}
        {% include 'views/eav/textInput.twig' %}
    {% endif %}
{% endfor %}

<div class="row">
    {{ C.Html.label('Область', false) }}
    {{ C.Html.dropDownList(
        'region_id', 'empty', regions,
        {'empty':'---------- область ----------', 'id':'id_region'})
    }}
</div>
<div class="row">
    {{ C.Html.activeLabel(model, 'city_id') }}
    {{ C.Html.activeDropDownList(
        model, 'city_id', {},
        {'empty':'------------ город ------------', 'id':'id_city'})
    }}
</div>

{{ C.Html.activeLabel(photo, 'image') }}
{{ C.Html.activeFileField(photo, 'image', {'multiple':'multiple','name':'images[]'}) }}
{{ C.Html.error(photo, 'image') }}

<div class="row buttons">
    {{ C.Html.submitButton() }}
</div>
{{ Yii.void(this.endWidget()) }}
</div>

<script type="text/javascript">
  $('#id_region').select2();
  $('#id_city').select2();
</script>

{% endblock content %}
      security-events: {% extends 'views/layouts/main_old.twig' %}

{% block content %}
<h3>Выберите категорию</h3>
{{ Yii.void(App.clientScript.registerCoreScript('jquery')) }}
{{ Yii.void(App.clientScript.registerCssFile(App.request.baseUrl~'/css/create.css')) }}
{{ Yii.void(App.clientScript.registerScriptFile(App.request.baseUrl~'/js/new.js', constant('CClientScript::POS_END'))) }}
<table class="category-select">
    <tr>
        <td class="first-level">
        <ul>
        {% for model in models %}
        <li>
        <a class="category-link" href="{{ App.request.baseUrl~'/ad/create/'~model.id }}"
            data-id="{{model.id}}">{{model.title}}</a>
        </li>
        {% endfor %}
        </ul>
        </td>
        <td class="second-level">
            <ul></ul>
        </td>
        <td class="third-level">
            <ul></ul>
        </td>
    </tr>
</table>
{% endblock content %}
      actions: {% extends 'views/layouts/main_old.twig' %}

{% block content %}
    {{ Yii.void(App.clientScript.registerCssFile(App.request.baseUrl~'/css/adview.css')) }}
    <h3>{{ model.title }}</h3>
    <div class="ad-author-and-city">
        <div class="ad-author">Автор: {{ model.author.name }}</div>
        <div class="ad-city">Город: {{ model.city.name }}</div>
        <div class="clear"></div>
    </div>
    {% for attrName, attrValue in model.getEavAttributes() %}
        <div style="color:green">
            {{ attrName }} ({{ model.getAttributeLabel(attrName) }}):{{ attrValue }}
        </div>
    {% endfor %}
    <div class="ad-description">Содержимое: {{ model.description }}</div>
    <div>{{ model.getAttributeLabel('price') }}
      {% if (model.isEavAttributeEmpty('price')) %} 0
      {% else %} {{ model.price }}
      {% endif %}
    </div>
    <div class="ad-added">Добавлено: {{ model.added }}</div>
    <div class="ad-images">
        {% for photo in model.photos %}
        <div class="ad-image">
            <img src="/images/thumb/big_{{photo.id}}_{{photo.name}}">
        </div>
        {% endfor %}
        <div class="clear"></div>
    </div>
{% endblock content %}
    name: <div class="col-sm-6 col-md-3 eav-attribute">
  {{ C.Html.label(
    form.model.getAttributeLabel(attribute)
    ~'('~form.model.getAttributeUnit(attribute)~'), от',
    attribute~'-min-id'
  ) }}
  {{ C.Html.textField(
    'search['~attribute~'][min]',
    value['min'],
    {
      'id': attribute~'-min-id',
      'class': 'form-control'
    }
  ) }}
</div>
<div class="col-sm-6 col-md-3 eav-attribute">
  {{ C.Html.label(
    form.model.getAttributeLabel(attribute)
    ~'('~form.model.getAttributeUnit(attribute)~'), до',
    attribute~'-max-id'
  ) }}
  {{ C.Html.textField(
    'search['~attribute~'][max]',
    value['max'],
    {
      'id': attribute~'-max-id',
      'class': 'form-control'
    }
  ) }}
</div>
    runs-on: <div class="row">
    {{ C.Html.activeLabel(model, key) }}
    {{ C.Html.activeDropDownList(model, key, model.getEavVariants(key), {'empty':'выбрать'}) }}
    {{ C.Html.error(model, key) }}
</div>

    steps: <div class="col-sm-6 col-md-3 eav-attribute">
  {{ C.Html.label(form.model.getAttributeLabel(attribute), attribute~'-id') }}
  {{ C.Html.dropDownList(
    'search['~attribute~']',
    value,
    form.model.getEavVariants(attribute),
    {
      'id': attribute~'-id',
      'empty':'выбрать',
      'class': 'form-control'
    }
  ) }}
</div>
      - name: <div class="row">
{{ C.Html.activeLabel(model, key) }}
{{ C.Html.activeTextField(model, key) }}
{{ C.Html.error(model, key) }}
</div>
        uses: {{ this.beginContent('//layouts/main') }}
<div id="content">
        {{ content }}
</div><!-- content -->
{{ Yii.void(this.endContent()) }}

      - name: {% extends 'views/layouts/main.twig' %}

{% block content %}
<div class="span-19">
        <div id="content">
                {% block view %}{% endblock view %}
        </div><!-- content -->
</div>
<div class="span-5 last">
        <div id="sidebar">
        {{ Yii.void(this.beginWidget('zii.widgets.CPortlet', {
                        'title':'Operations'
                }, true)) }}
        {{        this.widget('zii.widgets.CMenu', {
                        'items': [
                        {'label':'Home', 'url':['/post']},
                        {'label':'About', 'url':{0:'/site/page', 'view':'about'} },
                        {'label':'Contact', 'url':['/site/contact']}
                    ],
                        'htmlOptions': {'class':'operations'}
                }, true) }}
        {{        Yii.void(this.endWidget()) }}
        </div><!-- sidebar -->
</div>
{% endblock content %}
        run: <!DOCTYPE html>
<html lang="ru">

<head>
  <meta charset="UTF-8">
  <title>{% block title %}{{ this.pageTitle }}{% endblock title %}</title>
  <link rel="stylesheet" href="/css/bootstrap.min.css">
  <link rel="stylesheet" href="/css/navbar.css">
  <link rel="stylesheet" href="/css/common.css">
  {{ Yii.void(Yii.clientScript.registerCoreScript('jquery')) }}
  <script src="/js/bootstrap.min.js"></script>
  {% block auxres %}{% endblock auxres %}
</head>

<body>

<div class="container-fluid nopaddings">
  <div class="main-menu">
    <div class="center-block col-sm-12 col-md-10 col-lg-9 nofloat">
      <nav class="navbar navbar-default">
        <div class="navbar-header">
          <button type="button" class="navbar-toggle collapsed"
                  data-toggle="collapse" data-target="#navbar-collapse-1"
                  aria-expanded="false">
            <span class="sr-only">Toggle navigation</span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
          </button>
          <a class="navbar-brand" href="/">Доска объявлений</a>
        </div>
        <div class="collapse navbar-collapse" id="navbar-collapse-1">
          {{ this.widget('zii.widgets.CMenu', {
            'htmlOptions': {
              'class': 'nav navbar-nav navbar-right'
            },
            'items': [
            {'label':'Дать объявление', 'url':['/ad/new'], 'visible':not App.user.isGuest},
            {'label':'Вход', 'url':['/site/login'], 'visible': App.user.isGuest},
            {'label':'Выйти', 'url':['/site/logout'], 'visible':not App.user.isGuest }
            ]
          }, true) }}
        </div>
      </nav>
    </div>
  </div><!-- end of main-menu -->

  <div class="content">
    <div class="center-block col-sm-12 col-md-10 col-lg-9 nofloat">
      {% block content %}{% endblock content %}
    </div>
  </div>

  <footer class="footer">
    <div class="center-block col-sm-12 col-md-10 col-lg-9 text-muted
                nofloat footer-content">
      Classifieds 2016 &copy; All rights reserved<br>
      {{ Yii.powered() }}
    </div>
  </footer>
</div><!-- endof container -->

</body>
</html>

      # Build is not required unless generated source files are used
      # - name: <!DOCTYPE html>
<html lang="ru">

<head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8">

        <!-- blueprint CSS framework -->
        <link rel="stylesheet" type="text/css" href="{{App.request.baseUrl}}/css/screen.css" media="screen, projection">
        <link rel="stylesheet" type="text/css" href="{{App.request.baseUrl}}/css/print.css" media="print">
        <!--[if lt IE 8]>
        <link rel="stylesheet" type="text/css" href="{{App.request.baseUrl}}/css/ie.css" media="screen, projection">
        <![endif]-->

        <link rel="stylesheet" type="text/css" href="{{App.request.baseUrl}}/css/main.css">
        <link rel="stylesheet" type="text/css" href="{{App.request.baseUrl}}/css/form.css">
        {% block auxres %}{% endblock auxres %}
        <title>{% block title %}{{ this.pageTitle }}{% endblock title %}</title>
</head>

<body>

<div class="container" id="page">

        <div id="header">
                <div id="logo">{{ App.name }}</div>
        </div><!-- header -->

        <div id="mainmenu">
                {{ this.widget('zii.widgets.CMenu', {
                        'items': [
                                {'label':'На главную', 'url':['/site/index']},
                                {'label':'Дать объявление', 'url':['/ad/new'], 'visible':not App.user.isGuest},
                                {'label':'Вход', 'url':['/site/login'], 'visible': App.user.isGuest},
                                {'label':'Выйти', 'url':['/site/logout'], 'visible':not App.user.isGuest }
                        ]
                }, true) }}
        </div><!-- mainmenu -->
        <div id="content">
                {% block content %} {{ content }} {% endblock content %}
        </div>
        <div class="clear"></div>

        <div id="footer">
                Copyright &copy;
                {{ "now"|date('Y', timezone="Europe/Kiev") }}
                by My Company.<br>
                All Rights Reserved.<br>
                {{ Yii.powered() }}
        </div><!-- footer -->

</div><!-- page -->

</body>
</html>
      #   run: <?php
/* @var $this SiteController */

$this->pageTitle=Yii::app()->name . ' - About';
$this->breadcrumbs=array(
        'About',
);
?>
<h1>About</h1>

<p>This is a "static" page. You may change the content of this page
by updating the file <code><?php echo __FILE__; ?></code>.</p>

      - name: <div class="thumbnail preview">
  <a href="{{App.request.baseUrl~'/ad/view/'~data.id}}">
    <img src="{{App.request.baseUrl~'/images/thumb/'~
         'small_'~data.photos[0].id~'_'~data.photos[0].name}}"
         alt="{{data.title}}">
  </a>
  <h3>
    <a href="{{App.request.baseUrl~'/ad/view/'~data.id}}">
      {% if data.title|length > 25 %}
        {{data.title|slice(0,20)~'...'}}
      {% else %}
        {{data.title}}
      {% endif %}
    </a>
  </h3>
  {% if not data.isEavAttributeEmpty('price') %}
    <h4>{{ data.price }} {{ data.getAttributeUnit('price') }}.</h4>
  {% else %}
    <h4>0 {{ data.getAttributeUnit('price') }}.</h4>
  {% endif %}
</div>
        uses: <div class="ad-item clearfix">
  <a href="{{'/ad/view/'~data.id}}" class="pull-left ad-preview">
    <img src="{{ '/images/thumb/' ~
      'small_' ~ data.photos[0].id ~ '_' ~ data.photos[0].name}}"
         alt="{{ data.title }}"
         class="img-responsive">
  </a>

  <h3 class="col-sm-8 nopaddings">
    <a href="{{'/ad/view/'~data.id}}">{{ data.title }}</a>
  </h3>

  <div class="ad-price col-sm-4 nopaddings">
    {% if not data.isEavAttributeEmpty('price') %}
      {{ data.getEavAttribute('price')~' '~data.getAttributeUnit('price')~'.' }}
    {% endif %}
  </div>

  <div class="ad-city-added">
    <div class="ad-city">{{ data.city.name }}</div>
    <time class="ad-added text-muted">{{ data.added }}</time>
  </div>
</div>
        # Provide a unique ID to access the sarif output path
        id: <?php
/* @var $this SiteController */
/* @var $model ContactForm */
/* @var $form CActiveForm */

$this->pageTitle=Yii::app()->name . ' - Contact Us';
$this->breadcrumbs=array(
        'Contact',
);
?>

<h1>Contact Us</h1>

<?php if(Yii::app()->user->hasFlash('contact')): ?>

<div class="flash-success">
        <?php echo Yii::app()->user->getFlash('contact'); ?>
</div>

<?php else: ?>

<p>
If you have business inquiries or other questions, please fill out the following form to contact us. Thank you.
</p>

<div class="form">

<?php $form=$this->beginWidget('CActiveForm', array(
        'id'=>'contact-form',
        'enableClientValidation'=>true,
        'clientOptions'=>array(
                'validateOnSubmit'=>true,
        ),
)); ?>

        <p class="note">Fields with <span class="required">*</span> are required.</p>

        <?php echo $form->errorSummary($model); ?>

        <div class="row">
                <?php echo $form->labelEx($model,'name'); ?>
                <?php echo $form->textField($model,'name'); ?>
                <?php echo $form->error($model,'name'); ?>
        </div>

        <div class="row">
                <?php echo $form->labelEx($model,'email'); ?>
                <?php echo $form->textField($model,'email'); ?>
                <?php echo $form->error($model,'email'); ?>
        </div>

        <div class="row">
                <?php echo $form->labelEx($model,'subject'); ?>
                <?php echo $form->textField($model,'subject',array('size'=>60,'maxlength'=>128)); ?>
                <?php echo $form->error($model,'subject'); ?>
        </div>

        <div class="row">
                <?php echo $form->labelEx($model,'body'); ?>
                <?php echo $form->textArea($model,'body',array('rows'=>6, 'cols'=>50)); ?>
                <?php echo $form->error($model,'body'); ?>
        </div>

        <?php if(CCaptcha::checkRequirements()): ?>
        <div class="row">
                <?php echo $form->labelEx($model,'verifyCode'); ?>
                <div>
                <?php $this->widget('CCaptcha'); ?>
                <?php echo $form->textField($model,'verifyCode'); ?>
                </div>
                <div class="hint">Please enter the letters as they are shown in the image above.
                <br/>Letters are not case-sensitive.</div>
                <?php echo $form->error($model,'verifyCode'); ?>
        </div>
        <?php endif; ?>

        <div class="row buttons">
                <?php echo CHtml::submitButton('Submit'); ?>
        </div>

<?php $this->endWidget(); ?>

</div><!-- form -->

<?php endif; ?>
        with: <?php
/* @var $this SiteController */
/* @var $error array */

$this->pageTitle=Yii::app()->name . ' - Error';
$this->breadcrumbs=array(
        'Error',
);
?>

<h2>Error <?php echo $code; ?></h2>

<div class="error">
<?php echo CHtml::encode($message); ?>
</div>
          cmakeBuildDirectory: {% extends 'views/layouts/main.twig' %}

{% block title %}{{ App.name }} &mdash; главная{% endblock title %}

{% block auxres %}
  <link rel="stylesheet" href="/css/select2.css">
  <link rel="stylesheet" href="/css/select2-bootstrap.min.css">
  <link rel="stylesheet" href="{{ App.request.baseUrl~'/css/mainpage.css' }}">
  <script src="/js/select2.js"></script>
  <script src="/js/select2_locale_ru.js"></script>
  <script src="/js/main-page.js"></script>
  {{ Yii.void(App.clientScript.registerScriptFile(
  App.request.baseUrl~'/js/citiesDropDown.js', constant('CClientScript::POS_END'))) }}
{% endblock auxres %}

{% block content %}

  {{ C.Html.beginForm(
    App.createUrl('site/search'),
    'get',
    {'class': 'form-inline clearfix'}
  ) }}

  <div class="col-sm-4">
  {{ C.Html.activeTextField(form, 'word', {
    'name':'word',
    'placeholder':'поиск по фразе',
    'class': 'form-control'
  }) }}
  </div>

  {% if this.beginCache('dropDownRegion', {'duration': 1}) %}
    <div class="col-sm-3">
      {{ C.Html.activeDropDownList(
        form,
        'region_id',
        regionList,
        {
          'id': 'id_region',
          'prompt': 'область',
          'name': 'region',
          'class': 'form-control'
        }
      ) }}
    </div>
    {{ Yii.void(this.endCache()) }}
  {% endif %}

  <div class="col-sm-3">
    {{ C.Html.activeDropDownList(
      form,
      'city_id',
      {},
      {'id':'id_city', 'prompt':'город', 'name':'city', 'class': 'form-control'}
    ) }}
  </div>

  <div class="col-sm-2">
    {{ C.Html.submitButton(
      '',
      {
        'name': '',
        'value':'Найти',
        'class':'btn btn-default pull-right'
      }
    ) }}
  </div>

  {{ C.Html.endForm() }}

  {% if this.beginCache('roots', {'duration': 1}) %}
    <div class="roots clearfix">
      {% for category in categories %}
        {% if category.level == 1 %}
          <h3 class="root root-{{category.root}} col-sm-3">
            <a href="{{ App.request.baseUrl~'/site/search/'~category.id }}"
               data-root="{{category.root}}">
              {{ category.title }}
            </a>
          </h3>
        {% endif %}
      {% endfor %}
    </div>

    {% for i in 1..10 %}
      <div class="children root-{{ i }} clearfix" data-root="{{ i }}">
        {% for category in categories %}
          {% if category.level != 1 and category.root == i %}
            <div class="child root-{{category.root}} col-sm-3">
              <a href="{{ App.request.baseUrl~'/site/search/'~category.id }}">
                {{ category.title }}
              </a>
            </div>
          {% endif %}
        {% endfor %}
      </div>
    {% endfor %}
    {{ Yii.void(this.endCache()) }}
  {% endif %}

  {% if this.beginCache('_new', {'duration': 5}) %}
    <h2>Новые объявления</h2>
    <div class="new-ads clearfix">
      {{ this.widget('zii.widgets.CListView', {
        'dataProvider':dataProvider,
        'itemView':'_new',
        'summaryText':false
      }, true) }}
    </div>
    {{ Yii.void(this.endCache()) }}
  {% endif %}

{% endblock content %}
          # Ruleset file that will determine what checks will be run
          ruleset: {% extends 'views/layouts/main.twig' %}
{% block title %}{{ App.name }} : вход{% endblock title %}
{% block content %}

<h1>Login</h1>

<p>Please fill out the following form with your login credentials:</p>

<div class="form">

{% set form = this.beginWidget('CActiveForm', {
        'id':'login-form',
        'enableClientValidation':true,
        'enableAjaxValidation':true,
        'clientOptions':{'validateOnSubmit':true}
}, true) %}

        <p class="note">Fields with <span class="required">*</span> are required.</p>

        <div class="row">
                {{ form.labelEx(model,'username') }}
                {{ form.textField(model,'username') }}
                {{ form.error(model,'username') }}
        </div>

        <div class="row">
                {{ form.labelEx(model,'password') }}
                {{ form.passwordField(model,'password') }}
                {{ form.error(model,'password') }}
        </div>

        <div class="row rememberMe">
                {{ form.checkBox(model,'rememberMe') }}
                {{ form.label(model,'rememberMe') }}
                {{ form.error(model,'rememberMe') }}
        </div>

        <div class="row buttons">
                {{ C.Html.submitButton('Login') }}
        </div>

{{ Yii.void(this.endWidget()) }}

</div><!-- form -->

{% endblock content %}

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: {% extends 'views/layouts/main.twig' %}

{% block auxres %}
  <link rel="stylesheet" href="/css/select2.css">
  <link rel="stylesheet" href="/css/select2-bootstrap.min.css">
  <link rel="stylesheet" href="{{ App.request.baseUrl~'/css/search.css' }}">
  <script src="/js/select2.js"></script>
  <script src="/js/select2_locale_ru.js"></script>
  <script>
    $(function(){
      $("#id_region").select2();
      $("#id_city").select2();
    });
  </script>
{% endblock auxres %}

{% block title %}{{ App.name }} &mdash; поиск{% endblock title %}

{% block content %}

  {{ Yii.void(App.clientScript.registerScriptFile(
    App.request.baseUrl~'/js/citiesDropDown.js', constant('CClientScript::POS_END')))
  }}

  <!-- begin form -->
  {{ C.Html.beginForm(App.createUrl(
    'site/search/'~App.request.getQuery('id')),
    'get',
    {
      'class': 'clearfix'
    }
  ) }}

  <div class="clearfix">
    <div class="col-sm-6">
    {{ C.Html.textField(
      'word',
      form.word,
      {
        'placeholder':'поиск по фразе',
        'class': 'form-control'
      }
    ) }}
    </div>

    <div class="col-sm-3">
    {{ C.Html.dropDownList(
      'region_id',
      form.region_id ? form.region_id : 'empty',
      regions,
      {
        'empty':'область',
        'id':'id_region',
        'class': 'form-control'
      }
    ) }}
    </div>

    <div class="col-sm-3">
    {{ C.Html.dropDownList(
      'city_id',
      form.city_id ? form.city_id : 'empty',
      {},
      {
        'empty':'город',
        'id':'id_city',
        'class':'form-control'
      }
    ) }}
    </div>
  </div>

  <div class="eav-wrapper clearfix">
    <div class="form-group">
    {% for attribute, value in form.eav %}
      {% if form.model.getEavVariants(attribute) %}
        {% include 'views/eav/dropDownSearch.twig' %}
      {% else %}
        {% include 'views/eav/doubleTextInput.twig' %}
      {% endif %}
    {% endfor %}
    </div>
  </div>
  <div class="clearfix form-buttons pull-right">
    {{ C.Html.resetButton('очистить', {'class': 'btn btn-default'}) }}
    {{ C.Html.submitButton('Поиск', {'class': 'btn btn-default'}) }}
  </div>

  {{ C.Html.endForm() }}
  <!-- end form -->

  {{ this.widget('zii.widgets.CListView', {
    'dataProvider':dataProvider,
    'itemView':'_search_item',
    'htmlOptions': {
      'class': 'list-view clearfix'
    },
    'pager': {
      'htmlOptions': {
        'class': 'pagination'
      },
      'prevPageLabel': '&laquo;',
      'nextPageLabel': '&raquo;',
      'firstPageCssClass': 'hidden',
      'lastPageCssClass': 'hidden',
      'hiddenPageCssClass': 'disabled',
      'selectedPageCssClass': 'active',
      'header': ''
    },
    'pagerCssClass': 'my-pager'
  }, true) }}
{% endblock content %}
        uses: <div class="form">

{% set form = this.beginWidget('CActiveForm', {
        'id':'user-form',
        'enableAjaxValidation':true,
        'enableClientValidation':true
}, true) %}

        <p class="note">Fields with <span class="required">*</span> are required.</p>

        {{ form.errorSummary(model) }}

        <div class="row">
                {{ form.labelEx(model,'email') }}
                {{ form.textField(model, 'email', {'size':40,'maxlength':255}) }}
                {{ form.error(model,'email') }}
        </div>

        <div class="row">
                {{ form.labelEx(model,'password') }}
                {{ form.passwordField(model,'password', {'size':40,'maxlength':255}) }}
                {{ form.error(model,'password') }}
        </div>

        <div class="row">
                {{ form.labelEx(model,'name') }}
                {{ form.textField(model,'name',{'size':40,'maxlength':255}) }}
                {{ form.error(model,'name') }}
        </div>

        <div class="row">
                {{ form.labelEx(model,'phone') }}
                {{ form.textField(model,'phone',{'size':20,'maxlength':13}) }}
                {{ form.error(model,'phone') }}
        </div>

        <div class="row">
                {{ form.labelEx(model,'vk') }}
                {{ form.textField(model,'vk',{'size':40,'maxlength':255}) }}
                {{ form.error(model,'vk') }}
        </div>

        <div class="row">
                {{ form.labelEx(model,'skype') }}
                {{ form.textField(model,'skype',{'size':40,'maxlength':255}) }}
                {{ form.error(model,'skype') }}
        </div>

        <div class="row buttons">
                {{ C.Html.submitButton(model.isNewRecord ? 'Register' : 'Save') }}
        </div>

{{ Yii.void(this.endWidget()) }}

</div><!-- form -->
        with: {% extends 'views/layouts/main.twig' %}
{% block title %}{{ App.name }} : регистрация{% endblock title %}
{% block content %}
<h1>Register</h1>

{{ this.renderPartial('_form', {'model':model}) }}
{% endblock content %}
          sarif_file: #!/bin/bash

# Copyright (c) Helio Chissini de Castro, 2023. Part of the SW360 Frontend Project.

# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/

# SPDX-License-Identifier: EPL-2.0
# License-Filename: LICENSE

if [ -n "$http_proxy" ]; then
    npm config set proxy "${http_proxy}"
    npm config set https-proxy "$http_proxy"
elif [ -n "$HTTP_PROXY" ]; then
    npm config set proxy "$HTTP_PROXY"
    npm config set https-proxy "$HTTP_PROXY"
fi

      # Upload SARIF file as an Artifact to download and view
      # - name: #!/bin/bash

# Copyright (c) Helio Chissini de Castro, 2023. Part of the SW360 Frontend Project.

# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/

# SPDX-License-Identifier: EPL-2.0
# License-Filename: LICENSE

set -e

# Add directory as safe
git config --global --add safe.directory /workspaces/sw360-frontend

# Install npm packages to development
npm install
      #   uses: # Copyright (c) Helio Chissini de Castro, 2023. Part of the SW360 Frontend Project.

# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/

# SPDX-License-Identifier: EPL-2.0
# License-Filename: LICENSE

ARG VARIANT=20-bullseye-slim
FROM node:${VARIANT}

ARG USERNAME=devel
ARG HOMEDIR=/home/devel
ENV HOME=$HOMEDIR

# Install base utilities
RUN DEBIAN_FRONTEND=noninteractive apt update -qq \
    && DEBIAN_FRONTEND=noninteractive apt install -y --no-install-recommends \
    git \
    sudo \
    && rm -rf /var/lib/apt/lists/*

# Run with non privileged user
RUN useradd \
    --shell /bin/bash \
    --home-dir $HOMEDIR \
    --create-home $USERNAME

# sudo support
RUN echo "$USERNAME ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

USER ${USERNAME}
WORKDIR $HOMEDIR

COPY scripts/nodejs.sh /usr/local/bin/set_npm_proxy
RUN  sudo chmod +x /usr/local/bin/set_npm_proxy

# Expose port 3000
EXPOSE 3000
      #   with: // Copyright (c) Helio Chissini de Castro, 2023. Part of the SW360 Frontend Project.

// This program and the accompanying materials are made
// available under the terms of the Eclipse Public License 2.0
// which is available at https://www.eclipse.org/legal/epl-2.0/

// SPDX-License-Identifier: EPL-2.0
// License-Filename: LICENSE

{
    "name": "SW360 Frontend",
    "build": {
        "dockerfile": "Dockerfile",
        "args": {
            "VARIANT": "20-bullseye-slim"
        }
    },
    // Configure tool-specific properties.
    "customizations": {
        // Configure properties specific to VS Code.
        "vscode": {
            // Set *default* container specific settings.json values on container create.
            "settings": {
                "html.format.templating": true,
                "emmet.includeLanguages": {
                    "javascript": "javascriptreact"
                },
                "[javascript]": {
                    "editor.defaultFormatter": "esbenp.prettier-vscode"
                },
                "[scss]": {
                    "editor.defaultFormatter": "esbenp.prettier-vscode"
                },
                "[json]": {
                    "editor.defaultFormatter": "esbenp.prettier-vscode"
                },
                "[jsonc]": {
                    "editor.defaultFormatter": "esbenp.prettier-vscode"
                },
                "editor.formatOnSave": true,
                "eslint.alwaysShowStatus": true,
                "indentRainbow.colorOnWhiteSpaceOnly": true
            },
            // Add the IDs of extensions you want installed when the container is created.
            "extensions": [
                "aaron-bond.better-comments",
                "davidanson.vscode-markdownlint",
                "dbaeumer.vscode-eslint",
                "esbenp.prettier-vscode",
                "formulahendry.auto-close-tag",
                "formulahendry.auto-rename-tag",
                "naumovs.color-highlight",
                "oderwat.indent-rainbow",
                "shardulm94.trailing-spaces",
                "VisualStudioExptTeam.vscodeintellicode",
                "ms-azuretools.vscode-docker",
                "github.vscode-github-actions"
            ]
        }
    },
    // Use 'forwardPorts' to make a list of ports inside the container available locally.
    "forwardPorts": [3000, 9229],
    // Use 'postCreateCommand' to run commands after the container is created.
    "postCreateCommand": ".devcontainer/scripts/postCreateCommand.sh",
    // Set `remoteUser` to `root` to connect as root instead. More info: https://aka.ms/vscode-remote/containers/non-root.
    "remoteUser": "devel"
}
      #     name: FILE(GLOB SR_DEPENDENCIES duneanaobj/StandardRecord/*.h)

add_custom_command(# Rebuild if anything in StandardRecord/ changes
                   DEPENDS ${SR_DEPENDENCIES}
                   OUTPUT FlatRecord.cxx FlatRecord.h FwdDeclare.h
                   COMMAND gen_srproxy --flat -i duneanaobj/StandardRecord/StandardRecord.h -o FlatRecord --target caf::StandardRecord --include-path ${PROJECT_SOURCE_DIR}:$ENV{ROOT_INC} --output-path duneanaobj/StandardRecord/Flat/ --prolog ${CMAKE_CURRENT_SOURCE_DIR}/Prolog.h --extra-cflags ' -D_Float16=short -fsized-deallocation'
  )

include_directories($ENV{SRPROXY_INC})

if(DEFINED CETMODULES_CURRENT_PROJECT_NAME)
    cet_make_library(LIBRARY_NAME duneanaobj_StandardRecordFlat
                     SOURCE       FlatRecord.cxx
                     LIBRARIES    ${ROOT_BASIC_LIB_LIST} ROOT::TreePlayer
                     )

    if (DEFINED ENV{MRB_BUILDDIR} AND NOT "$ENV{MRB_BUILDDIR}" STREQUAL "")
        set(builddir $ENV{MRB_BUILDDIR}/duneanaobj)
    else()
        set(builddir ${CMAKE_BINARY_DIR})
    endif()
    install_headers(EXTRAS ${builddir}/duneanaobj/StandardRecord/Flat/FlatRecord.h ${builddir}/duneanaobj/StandardRecord/Flat/FwdDeclare.h)
else()
    add_library(duneanaobj_StandardRecordFlat
                FlatRecord.cxx)
    target_link_libraries(duneanaobj_StandardRecordFlat ${ROOT_BASIC_LIB_LIST} ROOT::TreePlayer)

    install(FILES ${CMAKE_BINARY_DIR}/duneanaobj/StandardRecord/Flat/FlatRecord.h ${CMAKE_BINARY_DIR}/duneanaobj/StandardRecord/Flat/FwdDeclare.h
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/duneanaobj)

endif()
      #     path: #include "duneanaobj/StandardRecord/SREnums.h"
