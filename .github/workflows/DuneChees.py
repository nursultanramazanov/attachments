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
          sarif_file: //
//  ActivityObserver.h
//  Clock Signal
//
//  Created by on 07/05/2018.
//  Copyright 2018. All rights reserved.
//

#pragma once

#include <cstdint>
#include <string>

namespace Activity {

/*!
        Provides a purely virtual base class for anybody that wants to receive notifications of
        'activity': any feedback from an emulated system which a user could perceive other than
        through the machine's native audio and video outputs.

        So: status LEDs, drive activity, etc. A receiver may choose to make appropriate noises
        and/or to show or unshow status indicators.
*/
class Observer {
        public:
                virtual ~Observer() = default;

                /// Provides hints as to the sort of information presented on an LED.
                enum LEDPresentation: uint8_t {
                        /// This LED informs the user of some sort of persistent state, e.g. scroll lock.
                        /// If this flag is absent then the LED describes an ephemeral state, such as media access.
                        Persistent = (1 << 0),
                };

                /// Announces to the receiver that there is an LED of name @c name.
                virtual void register_led([[maybe_unused]] const std::string &name, [[maybe_unused]] uint8_t presentation = 0) {}

                /// Announces to the receiver that there is a drive of name @c name.
                ///
                /// If a drive has the same name as an LED, that LED goes with this drive.
                virtual void register_drive([[maybe_unused]] const std::string &name) {}

                /// Informs the receiver of the new state of the LED with name @c name.
                virtual void set_led_status([[maybe_unused]] const std::string &name, [[maybe_unused]] bool lit) {}

                enum class DriveEvent {
                        StepNormal,
                        StepBelowZero,
                        StepBeyondMaximum
                };

                /// Informs the receiver that the named event just occurred for the drive with name @c name.
                virtual void announce_drive_event([[maybe_unused]] const std::string &name, [[maybe_unused]] DriveEvent event) {}

                /// Informs the receiver of the motor-on status of the drive with name @c name.
                virtual void set_drive_motor_status([[maybe_unused]] const std::string &name, [[maybe_unused]] bool is_on) {}
};

}
}
      #     name: //
//  ActivitySource.h
//  Clock Signal
//
//  Created by  on 07/05/2018.
//  Copyright 2018 . All rights reserved.
//

#pragma once

#include "Observer.hpp"

namespace Activity {

class Source {
        public:
                virtual void set_activity_observer(Observer *observer) = 0;
};

}
      #     path: fixed acidity,volatile acidity,citric acid,residual sugar,chlorides,free sulfur dioxide,total sulfur dioxide,density,pH,sulphates,alcohol,quality
7.4,0.7,0.0,1.9,0.076,11.0,34.0,0.9978,3.51,0.56,9.4,5
7.8,0.88,0.0,2.6,0.098,25.0,67.0,0.9968,3.2,0.68,9.8,5
7.8,0.76,0.04,2.3,0.092,15.0,54.0,0.997,3.26,0.65,9.8,5
11.2,0.28,0.56,1.9,0.075,17.0,60.0,0.998,3.16,0.58,9.8,6
7.4,0.7,0.0,1.9,0.076,11.0,34.0,0.9978,3.51,0.56,9.4,5
7.4,0.66,0.0,1.8,0.075,13.0,40.0,0.9978,3.51,0.56,9.4,5
7.9,0.6,0.06,1.6,0.069,15.0,59.0,0.9964,3.3,0.46,9.4,5
7.3,0.65,0.0,1.2,0.065,15.0,21.0,0.9946,3.39,0.47,10.0,7
7.8,0.58,0.02,2.0,0.073,9.0,18.0,0.9968,3.36,0.57,9.5,7
7.5,0.5,0.36,6.1,0.071,17.0,102.0,0.9978,3.35,0.8,10.5,5
6.7,0.58,0.08,1.8,0.09699999999999999,15.0,65.0,0.9959,3.28,0.54,9.2,5
7.5,0.5,0.36,6.1,0.071,17.0,102.0,0.9978,3.35,0.8,10.5,5
5.6,0.615,0.0,1.6,0.08900000000000001,16.0,59.0,0.9943,3.58,0.52,9.9,5
7.8,0.61,0.29,1.6,0.114,9.0,29.0,0.9974,3.26,1.56,9.1,5
8.9,0.62,0.18,3.8,0.17600000000000002,52.0,145.0,0.9986,3.16,0.88,9.2,5
8.9,0.62,0.19,3.9,0.17,51.0,148.0,0.9986,3.17,0.93,9.2,5
8.5,0.28,0.56,1.8,0.092,35.0,103.0,0.9969,3.3,0.75,10.5,7
8.1,0.56,0.28,1.7,0.368,16.0,56.0,0.9968,3.11,1.28,9.3,5
7.4,0.59,0.08,4.4,0.086,6.0,29.0,0.9974,3.38,0.5,9.0,4
7.9,0.32,0.51,1.8,0.341,17.0,56.0,0.9969,3.04,1.08,9.2,6
8.9,0.22,0.48,1.8,0.077,29.0,60.0,0.9968,3.39,0.53,9.4,6
7.6,0.39,0.31,2.3,0.08199999999999999,23.0,71.0,0.9982,3.52,0.65,9.7,5
7.9,0.43,0.21,1.6,0.106,10.0,37.0,0.9966,3.17,0.91,9.5,5
8.5,0.49,0.11,2.3,0.084,9.0,67.0,0.9968,3.17,0.53,9.4,5
6.9,0.4,0.14,2.4,0.085,21.0,40.0,0.9968,3.43,0.63,9.7,6
6.3,0.39,0.16,1.4,0.08,11.0,23.0,0.9955,3.34,0.56,9.3,5
7.6,0.41,0.24,1.8,0.08,4.0,11.0,0.9962,3.28,0.59,9.5,5
7.9,0.43,0.21,1.6,0.106,10.0,37.0,0.9966,3.17,0.91,9.5,5
7.1,0.71,0.0,1.9,0.08,14.0,35.0,0.9972,3.47,0.55,9.4,5
7.8,0.645,0.0,2.0,0.08199999999999999,8.0,16.0,0.9964,3.38,0.59,9.8,6
6.7,0.675,0.07,2.4,0.08900000000000001,17.0,82.0,0.9958,3.35,0.54,10.1,5
6.9,0.685,0.0,2.5,0.105,22.0,37.0,0.9966,3.46,0.57,10.6,6
8.3,0.655,0.12,2.3,0.083,15.0,113.0,0.9966,3.17,0.66,9.8,5
6.9,0.605,0.12,10.7,0.073,40.0,83.0,0.9993,3.45,0.52,9.4,6
5.2,0.32,0.25,1.8,0.10300000000000001,13.0,50.0,0.9957,3.38,0.55,9.2,5
7.8,0.645,0.0,5.5,0.086,5.0,18.0,0.9986,3.4,0.55,9.6,6
7.8,0.6,0.14,2.4,0.086,3.0,15.0,0.9975,3.42,0.6,10.8,6
8.1,0.38,0.28,2.1,0.066,13.0,30.0,0.9968,3.23,0.73,9.7,7
5.7,1.13,0.09,1.5,0.172,7.0,19.0,0.9940000000000001,3.5,0.48,9.8,4
7.3,0.45,0.36,5.9,0.07400000000000001,12.0,87.0,0.9978,3.33,0.83,10.5,5
7.3,0.45,0.36,5.9,0.07400000000000001,12.0,87.0,0.9978,3.33,0.83,10.5,5
8.8,0.61,0.3,2.8,0.08800000000000001,17.0,46.0,0.9976,3.26,0.51,9.3,4
7.5,0.49,0.2,2.6,0.332,8.0,14.0,0.9968,3.21,0.9,10.5,6
8.1,0.66,0.22,2.2,0.069,9.0,23.0,0.9968,3.3,1.2,10.3,5
6.8,0.67,0.02,1.8,0.05,5.0,11.0,0.9962,3.48,0.52,9.5,5
4.6,0.52,0.15,2.1,0.054000000000000006,8.0,65.0,0.9934,3.9,0.56,13.1,4
7.7,0.935,0.43,2.2,0.114,22.0,114.0,0.997,3.25,0.73,9.2,5
8.7,0.29,0.52,1.6,0.113,12.0,37.0,0.9969,3.25,0.58,9.5,5
6.4,0.4,0.23,1.6,0.066,5.0,12.0,0.9958,3.34,0.56,9.2,5
5.6,0.31,0.37,1.4,0.07400000000000001,12.0,96.0,0.9954,3.32,0.58,9.2,5
8.8,0.66,0.26,1.7,0.07400000000000001,4.0,23.0,0.9971,3.15,0.74,9.2,5
6.6,0.52,0.04,2.2,0.069,8.0,15.0,0.9956,3.4,0.63,9.4,6
6.6,0.5,0.04,2.1,0.068,6.0,14.0,0.9955,3.39,0.64,9.4,6
8.6,0.38,0.36,3.0,0.081,30.0,119.0,0.997,3.2,0.56,9.4,5
7.6,0.51,0.15,2.8,0.11,33.0,73.0,0.9955,3.17,0.63,10.2,6
7.7,0.62,0.04,3.8,0.084,25.0,45.0,0.9978,3.34,0.53,9.5,5
10.2,0.42,0.57,3.4,0.07,4.0,10.0,0.9971,3.04,0.63,9.6,5
7.5,0.63,0.12,5.1,0.111,50.0,110.0,0.9983,3.26,0.77,9.4,5
7.8,0.59,0.18,2.3,0.076,17.0,54.0,0.9975,3.43,0.59,10.0,5
7.3,0.39,0.31,2.4,0.07400000000000001,9.0,46.0,0.9962,3.41,0.54,9.4,6
8.8,0.4,0.4,2.2,0.079,19.0,52.0,0.998,3.44,0.64,9.2,5
7.7,0.69,0.49,1.8,0.115,20.0,112.0,0.9968,3.21,0.71,9.3,5
7.5,0.52,0.16,1.9,0.085,12.0,35.0,0.9968,3.38,0.62,9.5,7
7.0,0.735,0.05,2.0,0.081,13.0,54.0,0.9966,3.39,0.57,9.8,5
7.2,0.725,0.05,4.65,0.086,4.0,11.0,0.9962,3.41,0.39,10.9,5
7.2,0.725,0.05,4.65,0.086,4.0,11.0,0.9962,3.41,0.39,10.9,5
7.5,0.52,0.11,1.5,0.079,11.0,39.0,0.9968,3.42,0.58,9.6,5
6.6,0.705,0.07,1.6,0.076,6.0,15.0,0.9962,3.44,0.58,10.7,5
9.3,0.32,0.57,2.0,0.07400000000000001,27.0,65.0,0.9969,3.28,0.79,10.7,5
8.0,0.705,0.05,1.9,0.07400000000000001,8.0,19.0,0.9962,3.34,0.95,10.5,6
7.7,0.63,0.08,1.9,0.076,15.0,27.0,0.9967,3.32,0.54,9.5,6
7.7,0.67,0.23,2.1,0.08800000000000001,17.0,96.0,0.9962,3.32,0.48,9.5,5
7.7,0.69,0.22,1.9,0.084,18.0,94.0,0.9961,3.31,0.48,9.5,5
8.3,0.675,0.26,2.1,0.084,11.0,43.0,0.9976,3.31,0.53,9.2,4
9.7,0.32,0.54,2.5,0.094,28.0,83.0,0.9984,3.28,0.82,9.6,5
8.8,0.41,0.64,2.2,0.09300000000000001,9.0,42.0,0.9986,3.54,0.66,10.5,5
8.8,0.41,0.64,2.2,0.09300000000000001,9.0,42.0,0.9986,3.54,0.66,10.5,5
6.8,0.785,0.0,2.4,0.10400000000000001,14.0,30.0,0.9966,3.52,0.55,10.7,6
6.7,0.75,0.12,2.0,0.086,12.0,80.0,0.9958,3.38,0.52,10.1,5
8.3,0.625,0.2,1.5,0.08,27.0,119.0,0.9972,3.16,1.12,9.1,4
6.2,0.45,0.2,1.6,0.069,3.0,15.0,0.9958,3.41,0.56,9.2,5
7.8,0.43,0.7,1.9,0.46399999999999997,22.0,67.0,0.9974,3.13,1.28,9.4,5
7.4,0.5,0.47,2.0,0.086,21.0,73.0,0.997,3.36,0.57,9.1,5
7.3,0.67,0.26,1.8,0.401,16.0,51.0,0.9969,3.16,1.14,9.4,5
6.3,0.3,0.48,1.8,0.069,18.0,61.0,0.9959,3.44,0.78,10.3,6
6.9,0.55,0.15,2.2,0.076,19.0,40.0,0.9961,3.41,0.59,10.1,5
8.6,0.49,0.28,1.9,0.11,20.0,136.0,0.9972,2.93,1.95,9.9,6
7.7,0.49,0.26,1.9,0.062,9.0,31.0,0.9966,3.39,0.64,9.6,5
9.3,0.39,0.44,2.1,0.107,34.0,125.0,0.9978,3.14,1.22,9.5,5
7.0,0.62,0.08,1.8,0.076,8.0,24.0,0.9978,3.48,0.53,9.0,5
7.9,0.52,0.26,1.9,0.079,42.0,140.0,0.9964,3.23,0.54,9.5,5
8.6,0.49,0.28,1.9,0.11,20.0,136.0,0.9972,2.93,1.95,9.9,6
8.6,0.49,0.29,2.0,0.11,19.0,133.0,0.9972,2.93,1.98,9.8,5
7.7,0.49,0.26,1.9,0.062,9.0,31.0,0.9966,3.39,0.64,9.6,5
5.0,1.02,0.04,1.4,0.045,41.0,85.0,0.9938,3.75,0.48,10.5,4
4.7,0.6,0.17,2.3,0.057999999999999996,17.0,106.0,0.9932,3.85,0.6,12.9,6
6.8,0.775,0.0,3.0,0.102,8.0,23.0,0.9965,3.45,0.56,10.7,5
7.0,0.5,0.25,2.0,0.07,3.0,22.0,0.9963,3.25,0.63,9.2,5
7.6,0.9,0.06,2.5,0.079,5.0,10.0,0.9967,3.39,0.56,9.8,5
8.1,0.545,0.18,1.9,0.08,13.0,35.0,0.9972,3.3,0.59,9.0,6
8.3,0.61,0.3,2.1,0.084,11.0,50.0,0.9972,3.4,0.61,10.2,6
7.8,0.5,0.3,1.9,0.075,8.0,22.0,0.9959,3.31,0.56,10.4,6
8.1,0.545,0.18,1.9,0.08,13.0,35.0,0.9972,3.3,0.59,9.0,6
8.1,0.575,0.22,2.1,0.077,12.0,65.0,0.9967,3.29,0.51,9.2,5
7.2,0.49,0.24,2.2,0.07,5.0,36.0,0.996,3.33,0.48,9.4,5
8.1,0.575,0.22,2.1,0.077,12.0,65.0,0.9967,3.29,0.51,9.2,5
7.8,0.41,0.68,1.7,0.467,18.0,69.0,0.9973,3.08,1.31,9.3,5
6.2,0.63,0.31,1.7,0.08800000000000001,15.0,64.0,0.9969,3.46,0.79,9.3,5
8.0,0.33,0.53,2.5,0.091,18.0,80.0,0.9976,3.37,0.8,9.6,6
8.1,0.785,0.52,2.0,0.122,37.0,153.0,0.9969,3.21,0.69,9.3,5
7.8,0.56,0.19,1.8,0.10400000000000001,12.0,47.0,0.9964,3.19,0.93,9.5,5
8.4,0.62,0.09,2.2,0.084,11.0,108.0,0.9964,3.15,0.66,9.8,5
8.4,0.6,0.1,2.2,0.085,14.0,111.0,0.9964,3.15,0.66,9.8,5
10.1,0.31,0.44,2.3,0.08,22.0,46.0,0.9988,3.32,0.67,9.7,6
7.8,0.56,0.19,1.8,0.10400000000000001,12.0,47.0,0.9964,3.19,0.93,9.5,5
9.4,0.4,0.31,2.2,0.09,13.0,62.0,0.9966,3.07,0.63,10.5,6
8.3,0.54,0.28,1.9,0.077,11.0,40.0,0.9978,3.39,0.61,10.0,6
7.8,0.56,0.12,2.0,0.08199999999999999,7.0,28.0,0.997,3.37,0.5,9.4,6
8.8,0.55,0.04,2.2,0.11900000000000001,14.0,56.0,0.9962,3.21,0.6,10.9,6
7.0,0.69,0.08,1.8,0.09699999999999999,22.0,89.0,0.9959,3.34,0.54,9.2,6
7.3,1.07,0.09,1.7,0.17800000000000002,10.0,89.0,0.9962,3.3,0.57,9.0,5
8.8,0.55,0.04,2.2,0.11900000000000001,14.0,56.0,0.9962,3.21,0.6,10.9,6
7.3,0.695,0.0,2.5,0.075,3.0,13.0,0.998,3.49,0.52,9.2,5
8.0,0.71,0.0,2.6,0.08,11.0,34.0,0.9976,3.44,0.53,9.5,5
7.8,0.5,0.17,1.6,0.08199999999999999,21.0,102.0,0.996,3.39,0.48,9.5,5
9.0,0.62,0.04,1.9,0.146,27.0,90.0,0.9984,3.16,0.7,9.4,5
8.2,1.33,0.0,1.7,0.081,3.0,12.0,0.9964,3.53,0.49,10.9,5
8.1,1.33,0.0,1.8,0.08199999999999999,3.0,12.0,0.9964,3.54,0.48,10.9,5
8.0,0.59,0.16,1.8,0.065,3.0,16.0,0.9962,3.42,0.92,10.5,7
6.1,0.38,0.15,1.8,0.07200000000000001,6.0,19.0,0.9955,3.42,0.57,9.4,5
8.0,0.745,0.56,2.0,0.11800000000000001,30.0,134.0,0.9968,3.24,0.66,9.4,5
5.6,0.5,0.09,2.3,0.049,17.0,99.0,0.9937,3.63,0.63,13.0,5
5.6,0.5,0.09,2.3,0.049,17.0,99.0,0.9937,3.63,0.63,13.0,5
6.6,0.5,0.01,1.5,0.06,17.0,26.0,0.9952,3.4,0.58,9.8,6
7.9,1.04,0.05,2.2,0.084,13.0,29.0,0.9959,3.22,0.55,9.9,6
8.4,0.745,0.11,1.9,0.09,16.0,63.0,0.9965,3.19,0.82,9.6,5
8.3,0.715,0.15,1.8,0.08900000000000001,10.0,52.0,0.9968,3.23,0.77,9.5,5
7.2,0.415,0.36,2.0,0.081,13.0,45.0,0.9972,3.48,0.64,9.2,5
7.8,0.56,0.19,2.1,0.081,15.0,105.0,0.9962,3.33,0.54,9.5,5
7.8,0.56,0.19,2.0,0.081,17.0,108.0,0.9962,3.32,0.54,9.5,5
8.4,0.745,0.11,1.9,0.09,16.0,63.0,0.9965,3.19,0.82,9.6,5
8.3,0.715,0.15,1.8,0.08900000000000001,10.0,52.0,0.9968,3.23,0.77,9.5,5
5.2,0.34,0.0,1.8,0.05,27.0,63.0,0.9916,3.68,0.79,14.0,6
6.3,0.39,0.08,1.7,0.066,3.0,20.0,0.9954,3.34,0.58,9.4,5
5.2,0.34,0.0,1.8,0.05,27.0,63.0,0.9916,3.68,0.79,14.0,6
8.1,0.67,0.55,1.8,0.11699999999999999,32.0,141.0,0.9968,3.17,0.62,9.4,5
5.8,0.68,0.02,1.8,0.087,21.0,94.0,0.9944,3.54,0.52,10.0,5
7.6,0.49,0.26,1.6,0.23600000000000002,10.0,88.0,0.9968,3.11,0.8,9.3,5
6.9,0.49,0.1,2.3,0.07400000000000001,12.0,30.0,0.9959,3.42,0.58,10.2,6
8.2,0.4,0.44,2.8,0.08900000000000001,11.0,43.0,0.9975,3.53,0.61,10.5,6
7.3,0.33,0.47,2.1,0.077,5.0,11.0,0.9958,3.33,0.53,10.3,6
9.2,0.52,1.0,3.4,0.61,32.0,69.0,0.9996,2.74,2.0,9.4,4
7.5,0.6,0.03,1.8,0.095,25.0,99.0,0.995,3.35,0.54,10.1,5
7.5,0.6,0.03,1.8,0.095,25.0,99.0,0.995,3.35,0.54,10.1,5
7.1,0.43,0.42,5.5,0.07,29.0,129.0,0.9973,3.42,0.72,10.5,5
7.1,0.43,0.42,5.5,0.071,28.0,128.0,0.9973,3.42,0.71,10.5,5
7.1,0.43,0.42,5.5,0.07,29.0,129.0,0.9973,3.42,0.72,10.5,5
7.1,0.43,0.42,5.5,0.071,28.0,128.0,0.9973,3.42,0.71,10.5,5
7.1,0.68,0.0,2.2,0.073,12.0,22.0,0.9969,3.48,0.5,9.3,5
6.8,0.6,0.18,1.9,0.079,18.0,86.0,0.9968,3.59,0.57,9.3,6
7.6,0.95,0.03,2.0,0.09,7.0,20.0,0.9959,3.2,0.56,9.6,5
7.6,0.68,0.02,1.3,0.07200000000000001,9.0,20.0,0.9965,3.17,1.08,9.2,4
7.8,0.53,0.04,1.7,0.076,17.0,31.0,0.9964,3.33,0.56,10.0,6
7.4,0.6,0.26,7.3,0.07,36.0,121.0,0.9982,3.37,0.49,9.4,5
7.3,0.59,0.26,7.2,0.07,35.0,121.0,0.9981,3.37,0.49,9.4,5
7.8,0.63,0.48,1.7,0.1,14.0,96.0,0.9961,3.19,0.62,9.5,5
6.8,0.64,0.1,2.1,0.085,18.0,101.0,0.9956,3.34,0.52,10.2,5
7.3,0.55,0.03,1.6,0.07200000000000001,17.0,42.0,0.9956,3.37,0.48,9.0,4
6.8,0.63,0.07,2.1,0.08900000000000001,11.0,44.0,0.9953,3.47,0.55,10.4,6
7.5,0.705,0.24,1.8,0.36,15.0,63.0,0.9964,3.0,1.59,9.5,5
7.9,0.885,0.03,1.8,0.057999999999999996,4.0,8.0,0.9972,3.36,0.33,9.1,4
8.0,0.42,0.17,2.0,0.073,6.0,18.0,0.9972,3.29,0.61,9.2,6
8.0,0.42,0.17,2.0,0.073,6.0,18.0,0.9972,3.29,0.61,9.2,6
7.4,0.62,0.05,1.9,0.068,24.0,42.0,0.9961,3.42,0.57,11.5,6
7.3,0.38,0.21,2.0,0.08,7.0,35.0,0.9961,3.33,0.47,9.5,5
6.9,0.5,0.04,1.5,0.085,19.0,49.0,0.9958,3.35,0.78,9.5,5
7.3,0.38,0.21,2.0,0.08,7.0,35.0,0.9961,3.33,0.47,9.5,5
7.5,0.52,0.42,2.3,0.087,8.0,38.0,0.9972,3.58,0.61,10.5,6
7.0,0.805,0.0,2.5,0.068,7.0,20.0,0.9969,3.48,0.56,9.6,5
8.8,0.61,0.14,2.4,0.067,10.0,42.0,0.9969,3.19,0.59,9.5,5
8.8,0.61,0.14,2.4,0.067,10.0,42.0,0.9969,3.19,0.59,9.5,5
8.9,0.61,0.49,2.0,0.27,23.0,110.0,0.9972,3.12,1.02,9.3,5
7.2,0.73,0.02,2.5,0.076,16.0,42.0,0.9972,3.44,0.52,9.3,5
6.8,0.61,0.2,1.8,0.077,11.0,65.0,0.9971,3.54,0.58,9.3,5
6.7,0.62,0.21,1.9,0.079,8.0,62.0,0.997,3.52,0.58,9.3,6
8.9,0.31,0.57,2.0,0.111,26.0,85.0,0.9971,3.26,0.53,9.7,5
7.4,0.39,0.48,2.0,0.08199999999999999,14.0,67.0,0.9972,3.34,0.55,9.2,5
7.7,0.705,0.1,2.6,0.084,9.0,26.0,0.9976,3.39,0.49,9.7,5
7.9,0.5,0.33,2.0,0.084,15.0,143.0,0.9968,3.2,0.55,9.5,5
7.9,0.49,0.32,1.9,0.08199999999999999,17.0,144.0,0.9968,3.2,0.55,9.5,5
8.2,0.5,0.35,2.9,0.077,21.0,127.0,0.9976,3.23,0.62,9.4,5
6.4,0.37,0.25,1.9,0.07400000000000001,21.0,49.0,0.9974,3.57,0.62,9.8,6
6.8,0.63,0.12,3.8,0.099,16.0,126.0,0.9969,3.28,0.61,9.5,5
7.6,0.55,0.21,2.2,0.071,7.0,28.0,0.9964,3.28,0.55,9.7,5
7.6,0.55,0.21,2.2,0.071,7.0,28.0,0.9964,3.28,0.55,9.7,5
7.8,0.59,0.33,2.0,0.07400000000000001,24.0,120.0,0.9968,3.25,0.54,9.4,5
7.3,0.58,0.3,2.4,0.07400000000000001,15.0,55.0,0.9968,3.46,0.59,10.2,5
11.5,0.3,0.6,2.0,0.067,12.0,27.0,0.9981,3.11,0.97,10.1,6
5.4,0.835,0.08,1.2,0.046,13.0,93.0,0.9924,3.57,0.85,13.0,7
6.9,1.09,0.06,2.1,0.061,12.0,31.0,0.9948,3.51,0.43,11.4,4
9.6,0.32,0.47,1.4,0.055999999999999994,9.0,24.0,0.99695,3.22,0.82,10.3,7
8.8,0.37,0.48,2.1,0.09699999999999999,39.0,145.0,0.9975,3.04,1.03,9.3,5
6.8,0.5,0.11,1.5,0.075,16.0,49.0,0.99545,3.36,0.79,9.5,5
7.0,0.42,0.35,1.6,0.08800000000000001,16.0,39.0,0.9961,3.34,0.55,9.2,5
7.0,0.43,0.36,1.6,0.08900000000000001,14.0,37.0,0.99615,3.34,0.56,9.2,6
12.8,0.3,0.74,2.6,0.095,9.0,28.0,0.9994,3.2,0.77,10.8,7
12.8,0.3,0.74,2.6,0.095,9.0,28.0,0.9994,3.2,0.77,10.8,7
7.8,0.57,0.31,1.8,0.069,26.0,120.0,0.99625,3.29,0.53,9.3,5
7.8,0.44,0.28,2.7,0.1,18.0,95.0,0.9966,3.22,0.67,9.4,5
11.0,0.3,0.58,2.1,0.054000000000000006,7.0,19.0,0.998,3.31,0.88,10.5,7
9.7,0.53,0.6,2.0,0.039,5.0,19.0,0.99585,3.3,0.86,12.4,6
8.0,0.725,0.24,2.8,0.083,10.0,62.0,0.99685,3.35,0.56,10.0,6
11.6,0.44,0.64,2.1,0.059000000000000004,5.0,15.0,0.998,3.21,0.67,10.2,6
8.2,0.57,0.26,2.2,0.06,28.0,65.0,0.9959,3.3,0.43,10.1,5
7.8,0.735,0.08,2.4,0.092,10.0,41.0,0.9974,3.24,0.71,9.8,6
7.0,0.49,0.49,5.6,0.06,26.0,121.0,0.9974,3.34,0.76,10.5,5
8.7,0.625,0.16,2.0,0.10099999999999999,13.0,49.0,0.9962,3.14,0.57,11.0,5
8.1,0.725,0.22,2.2,0.07200000000000001,11.0,41.0,0.9967,3.36,0.55,9.1,5
7.5,0.49,0.19,1.9,0.076,10.0,44.0,0.9957,3.39,0.54,9.7,5
7.8,0.53,0.33,2.4,0.08,24.0,144.0,0.99655,3.3,0.6,9.5,5
7.8,0.34,0.37,2.0,0.08199999999999999,24.0,58.0,0.9964,3.34,0.59,9.4,6
7.4,0.53,0.26,2.0,0.10099999999999999,16.0,72.0,0.9957,3.15,0.57,9.4,5
6.8,0.61,0.04,1.5,0.057,5.0,10.0,0.99525,3.42,0.6,9.5,5
8.6,0.645,0.25,2.0,0.083,8.0,28.0,0.99815,3.28,0.6,10.0,6
8.4,0.635,0.36,2.0,0.08900000000000001,15.0,55.0,0.99745,3.31,0.57,10.4,4
7.7,0.43,0.25,2.6,0.073,29.0,63.0,0.99615,3.37,0.58,10.5,6
8.9,0.59,0.5,2.0,0.337,27.0,81.0,0.9964,3.04,1.61,9.5,6
9.0,0.82,0.14,2.6,0.08900000000000001,9.0,23.0,0.9984,3.39,0.63,9.8,5
7.7,0.43,0.25,2.6,0.073,29.0,63.0,0.99615,3.37,0.58,10.5,6
6.9,0.52,0.25,2.6,0.081,10.0,37.0,0.99685,3.46,0.5,11.0,5
5.2,0.48,0.04,1.6,0.054000000000000006,19.0,106.0,0.9927,3.54,0.62,12.2,7
8.0,0.38,0.06,1.8,0.078,12.0,49.0,0.99625,3.37,0.52,9.9,6
8.5,0.37,0.2,2.8,0.09,18.0,58.0,0.998,3.34,0.7,9.6,6
6.9,0.52,0.25,2.6,0.081,10.0,37.0,0.99685,3.46,0.5,11.0,5
8.2,1.0,0.09,2.3,0.065,7.0,37.0,0.99685,3.32,0.55,9.0,6
7.2,0.63,0.0,1.9,0.09699999999999999,14.0,38.0,0.99675,3.37,0.58,9.0,6
7.2,0.63,0.0,1.9,0.09699999999999999,14.0,38.0,0.99675,3.37,0.58,9.0,6
7.2,0.645,0.0,1.9,0.09699999999999999,15.0,39.0,0.99675,3.37,0.58,9.2,6
7.2,0.63,0.0,1.9,0.09699999999999999,14.0,38.0,0.99675,3.37,0.58,9.0,6
8.2,1.0,0.09,2.3,0.065,7.0,37.0,0.99685,3.32,0.55,9.0,6
8.9,0.635,0.37,1.7,0.263,5.0,62.0,0.9971,3.0,1.09,9.3,5
12.0,0.38,0.56,2.1,0.09300000000000001,6.0,24.0,0.99925,3.14,0.71,10.9,6
7.7,0.58,0.1,1.8,0.102,28.0,109.0,0.99565,3.08,0.49,9.8,6
15.0,0.21,0.44,2.2,0.075,10.0,24.0,1.00005,3.07,0.84,9.2,7
15.0,0.21,0.44,2.2,0.075,10.0,24.0,1.00005,3.07,0.84,9.2,7
7.3,0.66,0.0,2.0,0.084,6.0,23.0,0.9983,3.61,0.96,9.9,6
7.1,0.68,0.07,1.9,0.075,16.0,51.0,0.99685,3.38,0.52,9.5,5
8.2,0.6,0.17,2.3,0.07200000000000001,11.0,73.0,0.9963,3.2,0.45,9.3,5
7.7,0.53,0.06,1.7,0.07400000000000001,9.0,39.0,0.99615,3.35,0.48,9.8,6
7.3,0.66,0.0,2.0,0.084,6.0,23.0,0.9983,3.61,0.96,9.9,6
10.8,0.32,0.44,1.6,0.063,16.0,37.0,0.9985,3.22,0.78,10.0,6
7.1,0.6,0.0,1.8,0.07400000000000001,16.0,34.0,0.9972,3.47,0.7,9.9,6
11.1,0.35,0.48,3.1,0.09,5.0,21.0,0.9986,3.17,0.53,10.5,5
7.7,0.775,0.42,1.9,0.092,8.0,86.0,0.9959,3.23,0.59,9.5,5
7.1,0.6,0.0,1.8,0.07400000000000001,16.0,34.0,0.9972,3.47,0.7,9.9,6
8.0,0.57,0.23,3.2,0.073,17.0,119.0,0.99675,3.26,0.57,9.3,5
9.4,0.34,0.37,2.2,0.075,5.0,13.0,0.998,3.22,0.62,9.2,5
6.6,0.695,0.0,2.1,0.075,12.0,56.0,0.9968,3.49,0.67,9.2,5
7.7,0.41,0.76,1.8,0.611,8.0,45.0,0.9968,3.06,1.26,9.4,5
10.0,0.31,0.47,2.6,0.085,14.0,33.0,0.99965,3.36,0.8,10.5,7
7.9,0.33,0.23,1.7,0.077,18.0,45.0,0.99625,3.29,0.65,9.3,5
7.0,0.975,0.04,2.0,0.087,12.0,67.0,0.99565,3.35,0.6,9.4,4
8.0,0.52,0.03,1.7,0.07,10.0,35.0,0.99575,3.34,0.57,10.0,5
7.9,0.37,0.23,1.8,0.077,23.0,49.0,0.9963,3.28,0.67,9.3,5
12.5,0.56,0.49,2.4,0.064,5.0,27.0,0.9999,3.08,0.87,10.9,5
11.8,0.26,0.52,1.8,0.071,6.0,10.0,0.9968,3.2,0.72,10.2,7
8.1,0.87,0.0,3.3,0.096,26.0,61.0,1.00025,3.6,0.72,9.8,4
7.9,0.35,0.46,3.6,0.078,15.0,37.0,0.9973,3.35,0.86,12.8,8
6.9,0.54,0.04,3.0,0.077,7.0,27.0,0.9987,3.69,0.91,9.4,6
11.5,0.18,0.51,4.0,0.10400000000000001,4.0,23.0,0.9996,3.28,0.97,10.1,6
7.9,0.545,0.06,4.0,0.087,27.0,61.0,0.9965,3.36,0.67,10.7,6
11.5,0.18,0.51,4.0,0.10400000000000001,4.0,23.0,0.9996,3.28,0.97,10.1,6
10.9,0.37,0.58,4.0,0.071,17.0,65.0,0.99935,3.22,0.78,10.1,5
8.4,0.715,0.2,2.4,0.076,10.0,38.0,0.99735,3.31,0.64,9.4,5
7.5,0.65,0.18,7.0,0.08800000000000001,27.0,94.0,0.99915,3.38,0.77,9.4,5
7.9,0.545,0.06,4.0,0.087,27.0,61.0,0.9965,3.36,0.67,10.7,6
6.9,0.54,0.04,3.0,0.077,7.0,27.0,0.9987,3.69,0.91,9.4,6
11.5,0.18,0.51,4.0,0.10400000000000001,4.0,23.0,0.9996,3.28,0.97,10.1,6
10.3,0.32,0.45,6.4,0.073,5.0,13.0,0.9976,3.23,0.82,12.6,8
8.9,0.4,0.32,5.6,0.087,10.0,47.0,0.9991,3.38,0.77,10.5,7
11.4,0.26,0.44,3.6,0.071,6.0,19.0,0.9986,3.12,0.82,9.3,6
7.7,0.27,0.68,3.5,0.358,5.0,10.0,0.9972,3.25,1.08,9.9,7
7.6,0.52,0.12,3.0,0.067,12.0,53.0,0.9971,3.36,0.57,9.1,5
8.9,0.4,0.32,5.6,0.087,10.0,47.0,0.9991,3.38,0.77,10.5,7
9.9,0.59,0.07,3.4,0.102,32.0,71.0,1.00015,3.31,0.71,9.8,5
9.9,0.59,0.07,3.4,0.102,32.0,71.0,1.00015,3.31,0.71,9.8,5
12.0,0.45,0.55,2.0,0.073,25.0,49.0,0.9997,3.1,0.76,10.3,6
7.5,0.4,0.12,3.0,0.092,29.0,53.0,0.9967,3.37,0.7,10.3,6
8.7,0.52,0.09,2.5,0.091,20.0,49.0,0.9976,3.34,0.86,10.6,7
11.6,0.42,0.53,3.3,0.105,33.0,98.0,1.001,3.2,0.95,9.2,5
8.7,0.52,0.09,2.5,0.091,20.0,49.0,0.9976,3.34,0.86,10.6,7
11.0,0.2,0.48,2.0,0.34299999999999997,6.0,18.0,0.9979,3.3,0.71,10.5,5
10.4,0.55,0.23,2.7,0.091,18.0,48.0,0.9994,3.22,0.64,10.3,6
6.9,0.36,0.25,2.4,0.098,5.0,16.0,0.9964,3.41,0.6,10.1,6
13.3,0.34,0.52,3.2,0.094,17.0,53.0,1.0014,3.05,0.81,9.5,6
10.8,0.5,0.46,2.5,0.073,5.0,27.0,1.0001,3.05,0.64,9.5,5
10.6,0.83,0.37,2.6,0.086,26.0,70.0,0.9981,3.16,0.52,9.9,5
7.1,0.63,0.06,2.0,0.083,8.0,29.0,0.99855,3.67,0.73,9.6,5
7.2,0.65,0.02,2.3,0.094,5.0,31.0,0.9993,3.67,0.8,9.7,5
6.9,0.67,0.06,2.1,0.08,8.0,33.0,0.99845,3.68,0.71,9.6,5
7.5,0.53,0.06,2.6,0.086,20.0,44.0,0.9965,3.38,0.59,10.7,6
11.1,0.18,0.48,1.5,0.068,7.0,15.0,0.9973,3.22,0.64,10.1,6
8.3,0.705,0.12,2.6,0.092,12.0,28.0,0.9994,3.51,0.72,10.0,5
7.4,0.67,0.12,1.6,0.18600000000000003,5.0,21.0,0.996,3.39,0.54,9.5,5
8.4,0.65,0.6,2.1,0.11199999999999999,12.0,90.0,0.9973,3.2,0.52,9.2,5
10.3,0.53,0.48,2.5,0.063,6.0,25.0,0.9998,3.12,0.59,9.3,6
7.6,0.62,0.32,2.2,0.08199999999999999,7.0,54.0,0.9966,3.36,0.52,9.4,5
10.3,0.41,0.42,2.4,0.213,6.0,14.0,0.9994,3.19,0.62,9.5,6
10.3,0.43,0.44,2.4,0.214,5.0,12.0,0.9994,3.19,0.63,9.5,6
7.4,0.29,0.38,1.7,0.062,9.0,30.0,0.9968,3.41,0.53,9.5,6
10.3,0.53,0.48,2.5,0.063,6.0,25.0,0.9998,3.12,0.59,9.3,6
7.9,0.53,0.24,2.0,0.07200000000000001,15.0,105.0,0.996,3.27,0.54,9.4,6
9.0,0.46,0.31,2.8,0.09300000000000001,19.0,98.0,0.99815,3.32,0.63,9.5,6
8.6,0.47,0.3,3.0,0.076,30.0,135.0,0.9976,3.3,0.53,9.4,5
7.4,0.36,0.29,2.6,0.087,26.0,72.0,0.99645,3.39,0.68,11.0,5
7.1,0.35,0.29,2.5,0.096,20.0,53.0,0.9962,3.42,0.65,11.0,6
9.6,0.56,0.23,3.4,0.102,37.0,92.0,0.9996,3.3,0.65,10.1,5
9.6,0.77,0.12,2.9,0.08199999999999999,30.0,74.0,0.99865,3.3,0.64,10.4,6
9.8,0.66,0.39,3.2,0.083,21.0,59.0,0.9989,3.37,0.71,11.5,7
9.6,0.77,0.12,2.9,0.08199999999999999,30.0,74.0,0.99865,3.3,0.64,10.4,6
9.8,0.66,0.39,3.2,0.083,21.0,59.0,0.9989,3.37,0.71,11.5,7
9.3,0.61,0.26,3.4,0.09,25.0,87.0,0.99975,3.24,0.62,9.7,5
7.8,0.62,0.05,2.3,0.079,6.0,18.0,0.99735,3.29,0.63,9.3,5
10.3,0.59,0.42,2.8,0.09,35.0,73.0,0.9990000000000001,3.28,0.7,9.5,6
10.0,0.49,0.2,11.0,0.071,13.0,50.0,1.0015,3.16,0.69,9.2,6
10.0,0.49,0.2,11.0,0.071,13.0,50.0,1.0015,3.16,0.69,9.2,6
11.6,0.53,0.66,3.65,0.121,6.0,14.0,0.9978,3.05,0.74,11.5,7
10.3,0.44,0.5,4.5,0.107,5.0,13.0,0.998,3.28,0.83,11.5,5
13.4,0.27,0.62,2.6,0.08199999999999999,6.0,21.0,1.0002,3.16,0.67,9.7,6
10.7,0.46,0.39,2.0,0.061,7.0,15.0,0.9981,3.18,0.62,9.5,5
10.2,0.36,0.64,2.9,0.122,10.0,41.0,0.998,3.23,0.66,12.5,6
10.2,0.36,0.64,2.9,0.122,10.0,41.0,0.998,3.23,0.66,12.5,6
8.0,0.58,0.28,3.2,0.066,21.0,114.0,0.9973,3.22,0.54,9.4,6
8.4,0.56,0.08,2.1,0.105,16.0,44.0,0.9958,3.13,0.52,11.0,5
7.9,0.65,0.01,2.5,0.078,17.0,38.0,0.9963,3.34,0.74,11.7,7
11.9,0.695,0.53,3.4,0.128,7.0,21.0,0.9992,3.17,0.84,12.2,7
8.9,0.43,0.45,1.9,0.052000000000000005,6.0,16.0,0.9948,3.35,0.7,12.5,6
7.8,0.43,0.32,2.8,0.08,29.0,58.0,0.9974,3.31,0.64,10.3,5
12.4,0.49,0.58,3.0,0.10300000000000001,28.0,99.0,1.0008,3.16,1.0,11.5,6
12.5,0.28,0.54,2.3,0.08199999999999999,12.0,29.0,0.9997,3.11,1.36,9.8,7
12.2,0.34,0.5,2.4,0.066,10.0,21.0,1.0,3.12,1.18,9.2,6
10.6,0.42,0.48,2.7,0.065,5.0,18.0,0.9972,3.21,0.87,11.3,6
10.9,0.39,0.47,1.8,0.11800000000000001,6.0,14.0,0.9982,3.3,0.75,9.8,6
10.9,0.39,0.47,1.8,0.11800000000000001,6.0,14.0,0.9982,3.3,0.75,9.8,6
11.9,0.57,0.5,2.6,0.08199999999999999,6.0,32.0,1.0006,3.12,0.78,10.7,6
7.0,0.685,0.0,1.9,0.067,40.0,63.0,0.9979,3.6,0.81,9.9,5
6.6,0.815,0.02,2.7,0.07200000000000001,17.0,34.0,0.9955,3.58,0.89,12.3,7
13.8,0.49,0.67,3.0,0.09300000000000001,6.0,15.0,0.9986,3.02,0.93,12.0,6
9.6,0.56,0.31,2.8,0.08900000000000001,15.0,46.0,0.9979,3.11,0.92,10.0,6
9.1,0.785,0.0,2.6,0.09300000000000001,11.0,28.0,0.9994,3.36,0.86,9.4,6
10.7,0.67,0.22,2.7,0.107,17.0,34.0,1.0004,3.28,0.98,9.9,6
9.1,0.795,0.0,2.6,0.096,11.0,26.0,0.9994,3.35,0.83,9.4,6
7.7,0.665,0.0,2.4,0.09,8.0,19.0,0.9974,3.27,0.73,9.3,5
13.5,0.53,0.79,4.8,0.12,23.0,77.0,1.0018,3.18,0.77,13.0,5
6.1,0.21,0.4,1.4,0.066,40.5,165.0,0.9912,3.25,0.59,11.9,6
6.7,0.75,0.01,2.4,0.078,17.0,32.0,0.9955,3.55,0.61,12.8,6
11.5,0.41,0.52,3.0,0.08,29.0,55.0,1.0001,3.26,0.88,11.0,5
10.5,0.42,0.66,2.95,0.11599999999999999,12.0,29.0,0.997,3.24,0.75,11.7,7
11.9,0.43,0.66,3.1,0.109,10.0,23.0,1.0,3.15,0.85,10.4,7
12.6,0.38,0.66,2.6,0.08800000000000001,10.0,41.0,1.001,3.17,0.68,9.8,6
8.2,0.7,0.23,2.0,0.099,14.0,81.0,0.9973,3.19,0.7,9.4,5
8.6,0.45,0.31,2.6,0.086,21.0,50.0,0.9982,3.37,0.91,9.9,6
11.9,0.58,0.66,2.5,0.07200000000000001,6.0,37.0,0.9992,3.05,0.56,10.0,5
12.5,0.46,0.63,2.0,0.071,6.0,15.0,0.9988,2.99,0.87,10.2,5
12.8,0.615,0.66,5.8,0.083,7.0,42.0,1.0022,3.07,0.73,10.0,7
10.0,0.42,0.5,3.4,0.107,7.0,21.0,0.9979,3.26,0.93,11.8,6
12.8,0.615,0.66,5.8,0.083,7.0,42.0,1.0022,3.07,0.73,10.0,7
10.4,0.575,0.61,2.6,0.076,11.0,24.0,1.0,3.16,0.69,9.0,5
10.3,0.34,0.52,2.8,0.159,15.0,75.0,0.9998,3.18,0.64,9.4,5
9.4,0.27,0.53,2.4,0.07400000000000001,6.0,18.0,0.9962,3.2,1.13,12.0,7
6.9,0.765,0.02,2.3,0.063,35.0,63.0,0.9975,3.57,0.78,9.9,5
7.9,0.24,0.4,1.6,0.055999999999999994,11.0,25.0,0.9967,3.32,0.87,8.7,6
9.1,0.28,0.48,1.8,0.067,26.0,46.0,0.9967,3.32,1.04,10.6,6
7.4,0.55,0.22,2.2,0.106,12.0,72.0,0.9959,3.05,0.63,9.2,5
14.0,0.41,0.63,3.8,0.08900000000000001,6.0,47.0,1.0014,3.01,0.81,10.8,6
11.5,0.54,0.71,4.4,0.124,6.0,15.0,0.9984,3.01,0.83,11.8,7
11.5,0.45,0.5,3.0,0.078,19.0,47.0,1.0003,3.26,1.11,11.0,6
9.4,0.27,0.53,2.4,0.07400000000000001,6.0,18.0,0.9962,3.2,1.13,12.0,7
11.4,0.625,0.66,6.2,0.08800000000000001,6.0,24.0,0.9988,3.11,0.99,13.3,6
8.3,0.42,0.38,2.5,0.094,24.0,60.0,0.9979,3.31,0.7,10.8,6
8.3,0.26,0.42,2.0,0.08,11.0,27.0,0.9974,3.21,0.8,9.4,6
13.7,0.415,0.68,2.9,0.085,17.0,43.0,1.0014,3.06,0.8,10.0,6
8.3,0.26,0.42,2.0,0.08,11.0,27.0,0.9974,3.21,0.8,9.4,6
8.3,0.26,0.42,2.0,0.08,11.0,27.0,0.9974,3.21,0.8,9.4,6
7.7,0.51,0.28,2.1,0.087,23.0,54.0,0.998,3.42,0.74,9.2,5
7.4,0.63,0.07,2.4,0.09,11.0,37.0,0.9979,3.43,0.76,9.7,6
7.8,0.54,0.26,2.0,0.08800000000000001,23.0,48.0,0.9981,3.41,0.74,9.2,6
8.3,0.66,0.15,1.9,0.079,17.0,42.0,0.9972,3.31,0.54,9.6,6
7.8,0.46,0.26,1.9,0.08800000000000001,23.0,53.0,0.9981,3.43,0.74,9.2,6
9.6,0.38,0.31,2.5,0.096,16.0,49.0,0.9982,3.19,0.7,10.0,7
5.6,0.85,0.05,1.4,0.045,12.0,88.0,0.9924,3.56,0.82,12.9,8
13.7,0.415,0.68,2.9,0.085,17.0,43.0,1.0014,3.06,0.8,10.0,6
9.5,0.37,0.52,2.0,0.08199999999999999,6.0,26.0,0.998,3.18,0.51,9.5,5
8.4,0.665,0.61,2.0,0.11199999999999999,13.0,95.0,0.997,3.16,0.54,9.1,5
12.7,0.6,0.65,2.3,0.063,6.0,25.0,0.9997,3.03,0.57,9.9,5
12.0,0.37,0.76,4.2,0.066,7.0,38.0,1.0004,3.22,0.6,13.0,7
6.6,0.735,0.02,7.9,0.122,68.0,124.0,0.9994,3.47,0.53,9.9,5
11.5,0.59,0.59,2.6,0.087,13.0,49.0,0.9988,3.18,0.65,11.0,6
11.5,0.59,0.59,2.6,0.087,13.0,49.0,0.9988,3.18,0.65,11.0,6
8.7,0.765,0.22,2.3,0.064,9.0,42.0,0.9963,3.1,0.55,9.4,5
6.6,0.735,0.02,7.9,0.122,68.0,124.0,0.9994,3.47,0.53,9.9,5
7.7,0.26,0.3,1.7,0.059000000000000004,20.0,38.0,0.9949,3.29,0.47,10.8,6
12.2,0.48,0.54,2.6,0.085,19.0,64.0,1.0,3.1,0.61,10.5,6
11.4,0.6,0.49,2.7,0.085,10.0,41.0,0.9994,3.15,0.63,10.5,6
7.7,0.69,0.05,2.7,0.075,15.0,27.0,0.9974,3.26,0.61,9.1,5
8.7,0.31,0.46,1.4,0.059000000000000004,11.0,25.0,0.9966,3.36,0.76,10.1,6
9.8,0.44,0.47,2.5,0.063,9.0,28.0,0.9981,3.24,0.65,10.8,6
12.0,0.39,0.66,3.0,0.09300000000000001,12.0,30.0,0.9996,3.18,0.63,10.8,7
10.4,0.34,0.58,3.7,0.174,6.0,16.0,0.997,3.19,0.7,11.3,6
12.5,0.46,0.49,4.5,0.07,26.0,49.0,0.9981,3.05,0.57,9.6,4
9.0,0.43,0.34,2.5,0.08,26.0,86.0,0.9987,3.38,0.62,9.5,6
9.1,0.45,0.35,2.4,0.08,23.0,78.0,0.9987,3.38,0.62,9.5,5
7.1,0.735,0.16,1.9,0.1,15.0,77.0,0.9966,3.27,0.64,9.3,5
9.9,0.4,0.53,6.7,0.09699999999999999,6.0,19.0,0.9986,3.27,0.82,11.7,7
8.8,0.52,0.34,2.7,0.087,24.0,122.0,0.9982,3.26,0.61,9.5,5
8.6,0.725,0.24,6.6,0.11699999999999999,31.0,134.0,1.0014,3.32,1.07,9.3,5
10.6,0.48,0.64,2.2,0.111,6.0,20.0,0.997,3.26,0.66,11.7,6
7.0,0.58,0.12,1.9,0.091,34.0,124.0,0.9956,3.44,0.48,10.5,5
11.9,0.38,0.51,2.0,0.121,7.0,20.0,0.9996,3.24,0.76,10.4,6
6.8,0.77,0.0,1.8,0.066,34.0,52.0,0.9976,3.62,0.68,9.9,5
9.5,0.56,0.33,2.4,0.08900000000000001,35.0,67.0,0.9972,3.28,0.73,11.8,7
6.6,0.84,0.03,2.3,0.059000000000000004,32.0,48.0,0.9952,3.52,0.56,12.3,7
7.7,0.96,0.2,2.0,0.047,15.0,60.0,0.9955,3.36,0.44,10.9,5
10.5,0.24,0.47,2.1,0.066,6.0,24.0,0.9978,3.15,0.9,11.0,7
7.7,0.96,0.2,2.0,0.047,15.0,60.0,0.9955,3.36,0.44,10.9,5
6.6,0.84,0.03,2.3,0.059000000000000004,32.0,48.0,0.9952,3.52,0.56,12.3,7
6.4,0.67,0.08,2.1,0.045,19.0,48.0,0.9949,3.49,0.49,11.4,6
9.5,0.78,0.22,1.9,0.077,6.0,32.0,0.9988,3.26,0.56,10.6,6
9.1,0.52,0.33,1.3,0.07,9.0,30.0,0.9978,3.24,0.6,9.3,5
12.8,0.84,0.63,2.4,0.08800000000000001,13.0,35.0,0.9997,3.1,0.6,10.4,6
10.5,0.24,0.47,2.1,0.066,6.0,24.0,0.9978,3.15,0.9,11.0,7
7.8,0.55,0.35,2.2,0.07400000000000001,21.0,66.0,0.9974,3.25,0.56,9.2,5
11.9,0.37,0.69,2.3,0.078,12.0,24.0,0.9958,3.0,0.65,12.8,6
12.3,0.39,0.63,2.3,0.091,6.0,18.0,1.0004,3.16,0.49,9.5,5
10.4,0.41,0.55,3.2,0.076,22.0,54.0,0.9996,3.15,0.89,9.9,6
12.3,0.39,0.63,2.3,0.091,6.0,18.0,1.0004,3.16,0.49,9.5,5
8.0,0.67,0.3,2.0,0.06,38.0,62.0,0.9958,3.26,0.56,10.2,6
11.1,0.45,0.73,3.2,0.066,6.0,22.0,0.9986,3.17,0.66,11.2,6
10.4,0.41,0.55,3.2,0.076,22.0,54.0,0.9996,3.15,0.89,9.9,6
7.0,0.62,0.18,1.5,0.062,7.0,50.0,0.9951,3.08,0.6,9.3,5
12.6,0.31,0.72,2.2,0.07200000000000001,6.0,29.0,0.9987,2.88,0.82,9.8,8
11.9,0.4,0.65,2.15,0.068,7.0,27.0,0.9988,3.06,0.68,11.3,6
15.6,0.685,0.76,3.7,0.1,6.0,43.0,1.0032,2.95,0.68,11.2,7
10.0,0.44,0.49,2.7,0.077,11.0,19.0,0.9963,3.23,0.63,11.6,7
5.3,0.57,0.01,1.7,0.054000000000000006,5.0,27.0,0.9934,3.57,0.84,12.5,7
9.5,0.735,0.1,2.1,0.079,6.0,31.0,0.9986,3.23,0.56,10.1,6
12.5,0.38,0.6,2.6,0.081,31.0,72.0,0.9996,3.1,0.73,10.5,5
9.3,0.48,0.29,2.1,0.127,6.0,16.0,0.9968,3.22,0.72,11.2,5
8.6,0.53,0.22,2.0,0.1,7.0,27.0,0.9967,3.2,0.56,10.2,6
11.9,0.39,0.69,2.8,0.095,17.0,35.0,0.9994,3.1,0.61,10.8,6
11.9,0.39,0.69,2.8,0.095,17.0,35.0,0.9994,3.1,0.61,10.8,6
8.4,0.37,0.53,1.8,0.413,9.0,26.0,0.9979,3.06,1.06,9.1,6
6.8,0.56,0.03,1.7,0.084,18.0,35.0,0.9968,3.44,0.63,10.0,6
10.4,0.33,0.63,2.8,0.084,5.0,22.0,0.9998,3.26,0.74,11.2,7
7.0,0.23,0.4,1.6,0.063,21.0,67.0,0.9952,3.5,0.63,11.1,5
11.3,0.62,0.67,5.2,0.086,6.0,19.0,0.9988,3.22,0.69,13.4,8
8.9,0.59,0.39,2.3,0.095,5.0,22.0,0.9986,3.37,0.58,10.3,5
9.2,0.63,0.21,2.7,0.09699999999999999,29.0,65.0,0.9988,3.28,0.58,9.6,5
10.4,0.33,0.63,2.8,0.084,5.0,22.0,0.9998,3.26,0.74,11.2,7
11.6,0.58,0.66,2.2,0.07400000000000001,10.0,47.0,1.0008,3.25,0.57,9.0,3
9.2,0.43,0.52,2.3,0.083,14.0,23.0,0.9976,3.35,0.61,11.3,6
8.3,0.615,0.22,2.6,0.087,6.0,19.0,0.9982,3.26,0.61,9.3,5
11.0,0.26,0.68,2.55,0.085,10.0,25.0,0.997,3.18,0.61,11.8,5
8.1,0.66,0.7,2.2,0.098,25.0,129.0,0.9972,3.08,0.53,9.0,5
11.5,0.315,0.54,2.1,0.084,5.0,15.0,0.9987,2.98,0.7,9.2,6
10.0,0.29,0.4,2.9,0.098,10.0,26.0,1.0006,3.48,0.91,9.7,5
10.3,0.5,0.42,2.0,0.069,21.0,51.0,0.9982,3.16,0.72,11.5,6
8.8,0.46,0.45,2.6,0.065,7.0,18.0,0.9947,3.32,0.79,14.0,6
11.4,0.36,0.69,2.1,0.09,6.0,21.0,1.0,3.17,0.62,9.2,6
8.7,0.82,0.02,1.2,0.07,36.0,48.0,0.9952,3.2,0.58,9.8,5
13.0,0.32,0.65,2.6,0.09300000000000001,15.0,47.0,0.9996,3.05,0.61,10.6,5
9.6,0.54,0.42,2.4,0.081,25.0,52.0,0.997,3.2,0.71,11.4,6
12.5,0.37,0.55,2.6,0.083,25.0,68.0,0.9995,3.15,0.82,10.4,6
9.9,0.35,0.55,2.1,0.062,5.0,14.0,0.9971,3.26,0.79,10.6,5
10.5,0.28,0.51,1.7,0.08,10.0,24.0,0.9982,3.2,0.89,9.4,6
9.6,0.68,0.24,2.2,0.087,5.0,28.0,0.9988,3.14,0.6,10.2,5
9.3,0.27,0.41,2.0,0.091,6.0,16.0,0.998,3.28,0.7,9.7,5
10.4,0.24,0.49,1.8,0.075,6.0,20.0,0.9977,3.18,1.06,11.0,6
9.6,0.68,0.24,2.2,0.087,5.0,28.0,0.9988,3.14,0.6,10.2,5
9.4,0.685,0.11,2.7,0.077,6.0,31.0,0.9984,3.19,0.7,10.1,6
10.6,0.28,0.39,15.5,0.069,6.0,23.0,1.0026,3.12,0.66,9.2,5
9.4,0.3,0.56,2.8,0.08,6.0,17.0,0.9964,3.15,0.92,11.7,8
10.6,0.36,0.59,2.2,0.152,6.0,18.0,0.9986,3.04,1.05,9.4,5
10.6,0.36,0.6,2.2,0.152,7.0,18.0,0.9986,3.04,1.06,9.4,5
10.6,0.44,0.68,4.1,0.114,6.0,24.0,0.997,3.06,0.66,13.4,6
10.2,0.67,0.39,1.9,0.054000000000000006,6.0,17.0,0.9976,3.17,0.47,10.0,5
10.2,0.67,0.39,1.9,0.054000000000000006,6.0,17.0,0.9976,3.17,0.47,10.0,5
10.2,0.645,0.36,1.8,0.053,5.0,14.0,0.9982,3.17,0.42,10.0,6
11.6,0.32,0.55,2.8,0.081,35.0,67.0,1.0002,3.32,0.92,10.8,7
9.3,0.39,0.4,2.6,0.073,10.0,26.0,0.9984,3.34,0.75,10.2,6
9.3,0.775,0.27,2.8,0.078,24.0,56.0,0.9984,3.31,0.67,10.6,6
9.2,0.41,0.5,2.5,0.055,12.0,25.0,0.9952,3.34,0.79,13.3,7
8.9,0.4,0.51,2.6,0.052000000000000005,13.0,27.0,0.995,3.32,0.9,13.4,7
8.7,0.69,0.31,3.0,0.086,23.0,81.0,1.0002,3.48,0.74,11.6,6
6.5,0.39,0.23,8.3,0.051,28.0,91.0,0.9952,3.44,0.55,12.1,6
10.7,0.35,0.53,2.6,0.07,5.0,16.0,0.9972,3.15,0.65,11.0,8
7.8,0.52,0.25,1.9,0.081,14.0,38.0,0.9984,3.43,0.65,9.0,6
7.2,0.34,0.32,2.5,0.09,43.0,113.0,0.9966,3.32,0.79,11.1,5
10.7,0.35,0.53,2.6,0.07,5.0,16.0,0.9972,3.15,0.65,11.0,8
8.7,0.69,0.31,3.0,0.086,23.0,81.0,1.0002,3.48,0.74,11.6,6
7.8,0.52,0.25,1.9,0.081,14.0,38.0,0.9984,3.43,0.65,9.0,6
10.4,0.44,0.73,6.55,0.07400000000000001,38.0,76.0,0.9990000000000001,3.17,0.85,12.0,7
10.4,0.44,0.73,6.55,0.07400000000000001,38.0,76.0,0.9990000000000001,3.17,0.85,12.0,7
10.5,0.26,0.47,1.9,0.078,6.0,24.0,0.9976,3.18,1.04,10.9,7
10.5,0.24,0.42,1.8,0.077,6.0,22.0,0.9976,3.21,1.05,10.8,7
10.2,0.49,0.63,2.9,0.07200000000000001,10.0,26.0,0.9968,3.16,0.78,12.5,7
10.4,0.24,0.46,1.8,0.075,6.0,21.0,0.9976,3.25,1.02,10.8,7
11.2,0.67,0.55,2.3,0.084,6.0,13.0,1.0,3.17,0.71,9.5,6
10.0,0.59,0.31,2.2,0.09,26.0,62.0,0.9994,3.18,0.63,10.2,6
13.3,0.29,0.75,2.8,0.084,23.0,43.0,0.9986,3.04,0.68,11.4,7
12.4,0.42,0.49,4.6,0.073,19.0,43.0,0.9978,3.02,0.61,9.5,5
10.0,0.59,0.31,2.2,0.09,26.0,62.0,0.9994,3.18,0.63,10.2,6
10.7,0.4,0.48,2.1,0.125,15.0,49.0,0.998,3.03,0.81,9.7,6
10.5,0.51,0.64,2.4,0.107,6.0,15.0,0.9973,3.09,0.66,11.8,7
10.5,0.51,0.64,2.4,0.107,6.0,15.0,0.9973,3.09,0.66,11.8,7
8.5,0.655,0.49,6.1,0.122,34.0,151.0,1.001,3.31,1.14,9.3,5
12.5,0.6,0.49,4.3,0.1,5.0,14.0,1.001,3.25,0.74,11.9,6
10.4,0.61,0.49,2.1,0.2,5.0,16.0,0.9994,3.16,0.63,8.4,3
10.9,0.21,0.49,2.8,0.08800000000000001,11.0,32.0,0.9972,3.22,0.68,11.7,6
7.3,0.365,0.49,2.5,0.08800000000000001,39.0,106.0,0.9966,3.36,0.78,11.0,5
9.8,0.25,0.49,2.7,0.08800000000000001,15.0,33.0,0.9982,3.42,0.9,10.0,6
7.6,0.41,0.49,2.0,0.08800000000000001,16.0,43.0,0.998,3.48,0.64,9.1,5
8.2,0.39,0.49,2.3,0.099,47.0,133.0,0.9979,3.38,0.99,9.8,5
9.3,0.4,0.49,2.5,0.085,38.0,142.0,0.9978,3.22,0.55,9.4,5
9.2,0.43,0.49,2.4,0.086,23.0,116.0,0.9976,3.23,0.64,9.5,5
10.4,0.64,0.24,2.8,0.105,29.0,53.0,0.9998,3.24,0.67,9.9,5
7.3,0.365,0.49,2.5,0.08800000000000001,39.0,106.0,0.9966,3.36,0.78,11.0,5
7.0,0.38,0.49,2.5,0.09699999999999999,33.0,85.0,0.9962,3.39,0.77,11.4,6
8.2,0.42,0.49,2.6,0.084,32.0,55.0,0.9988,3.34,0.75,8.7,6
9.9,0.63,0.24,2.4,0.077,6.0,33.0,0.9974,3.09,0.57,9.4,5
9.1,0.22,0.24,2.1,0.078,1.0,28.0,0.9990000000000001,3.41,0.87,10.3,6
11.9,0.38,0.49,2.7,0.098,12.0,42.0,1.0004,3.16,0.61,10.3,5
11.9,0.38,0.49,2.7,0.098,12.0,42.0,1.0004,3.16,0.61,10.3,5
10.3,0.27,0.24,2.1,0.07200000000000001,15.0,33.0,0.9956,3.22,0.66,12.8,6
10.0,0.48,0.24,2.7,0.102,13.0,32.0,1.0,3.28,0.56,10.0,6
9.1,0.22,0.24,2.1,0.078,1.0,28.0,0.9990000000000001,3.41,0.87,10.3,6
9.9,0.63,0.24,2.4,0.077,6.0,33.0,0.9974,3.09,0.57,9.4,5
8.1,0.825,0.24,2.1,0.084,5.0,13.0,0.9972,3.37,0.77,10.7,6
12.9,0.35,0.49,5.8,0.066,5.0,35.0,1.0014,3.2,0.66,12.0,7
11.2,0.5,0.74,5.15,0.1,5.0,17.0,0.9996,3.22,0.62,11.2,5
9.2,0.59,0.24,3.3,0.10099999999999999,20.0,47.0,0.9988,3.26,0.67,9.6,5
9.5,0.46,0.49,6.3,0.064,5.0,17.0,0.9988,3.21,0.73,11.0,6
9.3,0.715,0.24,2.1,0.07,5.0,20.0,0.9966,3.12,0.59,9.9,5
11.2,0.66,0.24,2.5,0.085,16.0,53.0,0.9993,3.06,0.72,11.0,6
14.3,0.31,0.74,1.8,0.075,6.0,15.0,1.0008,2.86,0.79,8.4,6
9.1,0.47,0.49,2.6,0.094,38.0,106.0,0.9982,3.08,0.59,9.1,5
7.5,0.55,0.24,2.0,0.078,10.0,28.0,0.9983,3.45,0.78,9.5,6
10.6,0.31,0.49,2.5,0.067,6.0,21.0,0.9987,3.26,0.86,10.7,6
12.4,0.35,0.49,2.6,0.079,27.0,69.0,0.9994,3.12,0.75,10.4,6
9.0,0.53,0.49,1.9,0.171,6.0,25.0,0.9975,3.27,0.61,9.4,6
6.8,0.51,0.01,2.1,0.07400000000000001,9.0,25.0,0.9958,3.33,0.56,9.5,6
9.4,0.43,0.24,2.8,0.092,14.0,45.0,0.998,3.19,0.73,10.0,6
9.5,0.46,0.24,2.7,0.092,14.0,44.0,0.998,3.12,0.74,10.0,6
5.0,1.04,0.24,1.6,0.05,32.0,96.0,0.9934,3.74,0.62,11.5,5
15.5,0.645,0.49,4.2,0.095,10.0,23.0,1.00315,2.92,0.74,11.1,5
15.5,0.645,0.49,4.2,0.095,10.0,23.0,1.00315,2.92,0.74,11.1,5
10.9,0.53,0.49,4.6,0.11800000000000001,10.0,17.0,1.0002,3.07,0.56,11.7,6
15.6,0.645,0.49,4.2,0.095,10.0,23.0,1.00315,2.92,0.74,11.1,5
10.9,0.53,0.49,4.6,0.11800000000000001,10.0,17.0,1.0002,3.07,0.56,11.7,6
13.0,0.47,0.49,4.3,0.085,6.0,47.0,1.0021,3.3,0.68,12.7,6
12.7,0.6,0.49,2.8,0.075,5.0,19.0,0.9994,3.14,0.57,11.4,5
9.0,0.44,0.49,2.4,0.078,26.0,121.0,0.9978,3.23,0.58,9.2,5
9.0,0.54,0.49,2.9,0.094,41.0,110.0,0.9982,3.08,0.61,9.2,5
7.6,0.29,0.49,2.7,0.092,25.0,60.0,0.9971,3.31,0.61,10.1,6
13.0,0.47,0.49,4.3,0.085,6.0,47.0,1.0021,3.3,0.68,12.7,6
12.7,0.6,0.49,2.8,0.075,5.0,19.0,0.9994,3.14,0.57,11.4,5
8.7,0.7,0.24,2.5,0.226,5.0,15.0,0.9991,3.32,0.6,9.0,6
8.7,0.7,0.24,2.5,0.226,5.0,15.0,0.9991,3.32,0.6,9.0,6
9.8,0.5,0.49,2.6,0.25,5.0,20.0,0.9990000000000001,3.31,0.79,10.7,6
6.2,0.36,0.24,2.2,0.095,19.0,42.0,0.9946,3.57,0.57,11.7,6
11.5,0.35,0.49,3.3,0.07,10.0,37.0,1.0003,3.32,0.91,11.0,6
6.2,0.36,0.24,2.2,0.095,19.0,42.0,0.9946,3.57,0.57,11.7,6
10.2,0.24,0.49,2.4,0.075,10.0,28.0,0.9978,3.14,0.61,10.4,5
10.5,0.59,0.49,2.1,0.07,14.0,47.0,0.9991,3.3,0.56,9.6,4
10.6,0.34,0.49,3.2,0.078,20.0,78.0,0.9992,3.19,0.7,10.0,6
12.3,0.27,0.49,3.1,0.079,28.0,46.0,0.9993,3.2,0.8,10.2,6
9.9,0.5,0.24,2.3,0.10300000000000001,6.0,14.0,0.9978,3.34,0.52,10.0,4
8.8,0.44,0.49,2.8,0.083,18.0,111.0,0.9982,3.3,0.6,9.5,5
8.8,0.47,0.49,2.9,0.085,17.0,110.0,0.9982,3.29,0.6,9.8,5
10.6,0.31,0.49,2.2,0.063,18.0,40.0,0.9976,3.14,0.51,9.8,6
12.3,0.5,0.49,2.2,0.08900000000000001,5.0,14.0,1.0002,3.19,0.44,9.6,5
12.3,0.5,0.49,2.2,0.08900000000000001,5.0,14.0,1.0002,3.19,0.44,9.6,5
11.7,0.49,0.49,2.2,0.083,5.0,15.0,1.0,3.19,0.43,9.2,5
12.0,0.28,0.49,1.9,0.07400000000000001,10.0,21.0,0.9976,2.98,0.66,9.9,7
11.8,0.33,0.49,3.4,0.09300000000000001,54.0,80.0,1.0002,3.3,0.76,10.7,7
7.6,0.51,0.24,2.4,0.091,8.0,38.0,0.998,3.47,0.66,9.6,6
11.1,0.31,0.49,2.7,0.094,16.0,47.0,0.9986,3.12,1.02,10.6,7
7.3,0.73,0.24,1.9,0.10800000000000001,18.0,102.0,0.9967,3.26,0.59,9.3,5
5.0,0.42,0.24,2.0,0.06,19.0,50.0,0.9917,3.72,0.74,14.0,8
10.2,0.29,0.49,2.6,0.059000000000000004,5.0,13.0,0.9976,3.05,0.74,10.5,7
9.0,0.45,0.49,2.6,0.084,21.0,75.0,0.9987,3.35,0.57,9.7,5
6.6,0.39,0.49,1.7,0.07,23.0,149.0,0.9922,3.12,0.5,11.5,6
9.0,0.45,0.49,2.6,0.084,21.0,75.0,0.9987,3.35,0.57,9.7,5
9.9,0.49,0.58,3.5,0.094,9.0,43.0,1.0004,3.29,0.58,9.0,5
7.9,0.72,0.17,2.6,0.096,20.0,38.0,0.9978,3.4,0.53,9.5,5
8.9,0.595,0.41,7.9,0.086,30.0,109.0,0.9998,3.27,0.57,9.3,5
12.4,0.4,0.51,2.0,0.059000000000000004,6.0,24.0,0.9994,3.04,0.6,9.3,6
11.9,0.58,0.58,1.9,0.071,5.0,18.0,0.998,3.09,0.63,10.0,6
8.5,0.585,0.18,2.1,0.078,5.0,30.0,0.9967,3.2,0.48,9.8,6
12.7,0.59,0.45,2.3,0.08199999999999999,11.0,22.0,1.0,3.0,0.7,9.3,6
8.2,0.915,0.27,2.1,0.08800000000000001,7.0,23.0,0.9962,3.26,0.47,10.0,4
13.2,0.46,0.52,2.2,0.071,12.0,35.0,1.0006,3.1,0.56,9.0,6
7.7,0.835,0.0,2.6,0.081,6.0,14.0,0.9975,3.3,0.52,9.3,5
13.2,0.46,0.52,2.2,0.071,12.0,35.0,1.0006,3.1,0.56,9.0,6
8.3,0.58,0.13,2.9,0.096,14.0,63.0,0.9984,3.17,0.62,9.1,6
8.3,0.6,0.13,2.6,0.085,6.0,24.0,0.9984,3.31,0.59,9.2,6
9.4,0.41,0.48,4.6,0.07200000000000001,10.0,20.0,0.9973,3.34,0.79,12.2,7
8.8,0.48,0.41,3.3,0.092,26.0,52.0,0.9982,3.31,0.53,10.5,6
10.1,0.65,0.37,5.1,0.11,11.0,65.0,1.0026,3.32,0.64,10.4,6
6.3,0.36,0.19,3.2,0.075,15.0,39.0,0.9956,3.56,0.52,12.7,6
8.8,0.24,0.54,2.5,0.083,25.0,57.0,0.9983,3.39,0.54,9.2,5
13.2,0.38,0.55,2.7,0.081,5.0,16.0,1.0006,2.98,0.54,9.4,5
7.5,0.64,0.0,2.4,0.077,18.0,29.0,0.9965,3.32,0.6,10.0,6
8.2,0.39,0.38,1.5,0.057999999999999996,10.0,29.0,0.9962,3.26,0.74,9.8,5
9.2,0.755,0.18,2.2,0.14800000000000002,10.0,103.0,0.9969,2.87,1.36,10.2,6
9.6,0.6,0.5,2.3,0.079,28.0,71.0,0.9997,3.5,0.57,9.7,5
9.6,0.6,0.5,2.3,0.079,28.0,71.0,0.9997,3.5,0.57,9.7,5
11.5,0.31,0.51,2.2,0.079,14.0,28.0,0.9982,3.03,0.93,9.8,6
11.4,0.46,0.5,2.7,0.122,4.0,17.0,1.0006,3.13,0.7,10.2,5
11.3,0.37,0.41,2.3,0.08800000000000001,6.0,16.0,0.9988,3.09,0.8,9.3,5
8.3,0.54,0.24,3.4,0.076,16.0,112.0,0.9976,3.27,0.61,9.4,5
8.2,0.56,0.23,3.4,0.078,14.0,104.0,0.9976,3.28,0.62,9.4,5
10.0,0.58,0.22,1.9,0.08,9.0,32.0,0.9974,3.13,0.55,9.5,5
7.9,0.51,0.25,2.9,0.077,21.0,45.0,0.9974,3.49,0.96,12.1,6
6.8,0.69,0.0,5.6,0.124,21.0,58.0,0.9997,3.46,0.72,10.2,5
6.8,0.69,0.0,5.6,0.124,21.0,58.0,0.9997,3.46,0.72,10.2,5
8.8,0.6,0.29,2.2,0.098,5.0,15.0,0.9988,3.36,0.49,9.1,5
8.8,0.6,0.29,2.2,0.098,5.0,15.0,0.9988,3.36,0.49,9.1,5
8.7,0.54,0.26,2.5,0.09699999999999999,7.0,31.0,0.9976,3.27,0.6,9.3,6
7.6,0.685,0.23,2.3,0.111,20.0,84.0,0.9964,3.21,0.61,9.3,5
8.7,0.54,0.26,2.5,0.09699999999999999,7.0,31.0,0.9976,3.27,0.6,9.3,6
10.4,0.28,0.54,2.7,0.105,5.0,19.0,0.9988,3.25,0.63,9.5,5
7.6,0.41,0.14,3.0,0.087,21.0,43.0,0.9964,3.32,0.57,10.5,6
10.1,0.935,0.22,3.4,0.105,11.0,86.0,1.001,3.43,0.64,11.3,4
7.9,0.35,0.21,1.9,0.073,46.0,102.0,0.9964,3.27,0.58,9.5,5
8.7,0.84,0.0,1.4,0.065,24.0,33.0,0.9954,3.27,0.55,9.7,5
9.6,0.88,0.28,2.4,0.086,30.0,147.0,0.9979,3.24,0.53,9.4,5
9.5,0.885,0.27,2.3,0.084,31.0,145.0,0.9978,3.24,0.53,9.4,5
7.7,0.915,0.12,2.2,0.14300000000000002,7.0,23.0,0.9964,3.35,0.65,10.2,7
8.9,0.29,0.35,1.9,0.067,25.0,57.0,0.997,3.18,1.36,10.3,6
9.9,0.54,0.45,2.3,0.071,16.0,40.0,0.9991,3.39,0.62,9.4,5
9.5,0.59,0.44,2.3,0.071,21.0,68.0,0.9992,3.46,0.63,9.5,5
9.9,0.54,0.45,2.3,0.071,16.0,40.0,0.9991,3.39,0.62,9.4,5
9.5,0.59,0.44,2.3,0.071,21.0,68.0,0.9992,3.46,0.63,9.5,5
9.9,0.54,0.45,2.3,0.071,16.0,40.0,0.9991,3.39,0.62,9.4,5
7.8,0.64,0.1,6.0,0.115,5.0,11.0,0.9984,3.37,0.69,10.1,7
7.3,0.67,0.05,3.6,0.107,6.0,20.0,0.9972,3.4,0.63,10.1,5
8.3,0.845,0.01,2.2,0.07,5.0,14.0,0.9967,3.32,0.58,11.0,4
8.7,0.48,0.3,2.8,0.066,10.0,28.0,0.9964,3.33,0.67,11.2,7
6.7,0.42,0.27,8.6,0.068,24.0,148.0,0.9948,3.16,0.57,11.3,6
10.7,0.43,0.39,2.2,0.106,8.0,32.0,0.9986,2.89,0.5,9.6,5
9.8,0.88,0.25,2.5,0.10400000000000001,35.0,155.0,1.001,3.41,0.67,11.2,5
15.9,0.36,0.65,7.5,0.096,22.0,71.0,0.9976,2.98,0.84,14.9,5
9.4,0.33,0.59,2.8,0.079,9.0,30.0,0.9976,3.12,0.54,12.0,6
8.6,0.47,0.47,2.4,0.07400000000000001,7.0,29.0,0.9979,3.08,0.46,9.5,5
9.7,0.55,0.17,2.9,0.087,20.0,53.0,1.0004,3.14,0.61,9.4,5
10.7,0.43,0.39,2.2,0.106,8.0,32.0,0.9986,2.89,0.5,9.6,5
12.0,0.5,0.59,1.4,0.073,23.0,42.0,0.998,2.92,0.68,10.5,7
7.2,0.52,0.07,1.4,0.07400000000000001,5.0,20.0,0.9973,3.32,0.81,9.6,6
7.1,0.84,0.02,4.4,0.096,5.0,13.0,0.997,3.41,0.57,11.0,4
7.2,0.52,0.07,1.4,0.07400000000000001,5.0,20.0,0.9973,3.32,0.81,9.6,6
7.5,0.42,0.31,1.6,0.08,15.0,42.0,0.9978,3.31,0.64,9.0,5
7.2,0.57,0.06,1.6,0.076,9.0,27.0,0.9972,3.36,0.7,9.6,6
10.1,0.28,0.46,1.8,0.05,5.0,13.0,0.9974,3.04,0.79,10.2,6
12.1,0.4,0.52,2.0,0.092,15.0,54.0,1.0,3.03,0.66,10.2,5
9.4,0.59,0.14,2.0,0.084,25.0,48.0,0.9981,3.14,0.56,9.7,5
8.3,0.49,0.36,1.8,0.222,6.0,16.0,0.998,3.18,0.6,9.5,6
11.3,0.34,0.45,2.0,0.08199999999999999,6.0,15.0,0.9988,2.94,0.66,9.2,6
10.0,0.73,0.43,2.3,0.059000000000000004,15.0,31.0,0.9966,3.15,0.57,11.0,5
11.3,0.34,0.45,2.0,0.08199999999999999,6.0,15.0,0.9988,2.94,0.66,9.2,6
6.9,0.4,0.24,2.5,0.083,30.0,45.0,0.9959,3.26,0.58,10.0,5
8.2,0.73,0.21,1.7,0.07400000000000001,5.0,13.0,0.9968,3.2,0.52,9.5,5
9.8,1.24,0.34,2.0,0.079,32.0,151.0,0.998,3.15,0.53,9.5,5
8.2,0.73,0.21,1.7,0.07400000000000001,5.0,13.0,0.9968,3.2,0.52,9.5,5
10.8,0.4,0.41,2.2,0.084,7.0,17.0,0.9984,3.08,0.67,9.3,6
9.3,0.41,0.39,2.2,0.064,12.0,31.0,0.9984,3.26,0.65,10.2,5
10.8,0.4,0.41,2.2,0.084,7.0,17.0,0.9984,3.08,0.67,9.3,6
8.6,0.8,0.11,2.3,0.084,12.0,31.0,0.9979,3.4,0.48,9.9,5
8.3,0.78,0.1,2.6,0.081,45.0,87.0,0.9983,3.48,0.53,10.0,5
10.8,0.26,0.45,3.3,0.06,20.0,49.0,0.9972,3.13,0.54,9.6,5
13.3,0.43,0.58,1.9,0.07,15.0,40.0,1.0004,3.06,0.49,9.0,5
8.0,0.45,0.23,2.2,0.094,16.0,29.0,0.9962,3.21,0.49,10.2,6
8.5,0.46,0.31,2.25,0.078,32.0,58.0,0.998,3.33,0.54,9.8,5
8.1,0.78,0.23,2.6,0.059000000000000004,5.0,15.0,0.997,3.37,0.56,11.3,5
9.8,0.98,0.32,2.3,0.078,35.0,152.0,0.998,3.25,0.48,9.4,5
8.1,0.78,0.23,2.6,0.059000000000000004,5.0,15.0,0.997,3.37,0.56,11.3,5
7.1,0.65,0.18,1.8,0.07,13.0,40.0,0.997,3.44,0.6,9.1,5
9.1,0.64,0.23,3.1,0.095,13.0,38.0,0.9998,3.28,0.59,9.7,5
7.7,0.66,0.04,1.6,0.039,4.0,9.0,0.9962,3.4,0.47,9.4,5
8.1,0.38,0.48,1.8,0.157,5.0,17.0,0.9976,3.3,1.05,9.4,5
7.4,1.185,0.0,4.25,0.09699999999999999,5.0,14.0,0.9966,3.63,0.54,10.7,3
9.2,0.92,0.24,2.6,0.087,12.0,93.0,0.9998,3.48,0.54,9.8,5
8.6,0.49,0.51,2.0,0.42200000000000004,16.0,62.0,0.9979,3.03,1.17,9.0,5
9.0,0.48,0.32,2.8,0.084,21.0,122.0,0.9984,3.32,0.62,9.4,5
9.0,0.47,0.31,2.7,0.084,24.0,125.0,0.9984,3.31,0.61,9.4,5
5.1,0.47,0.02,1.3,0.034,18.0,44.0,0.9921,3.9,0.62,12.8,6
7.0,0.65,0.02,2.1,0.066,8.0,25.0,0.9972,3.47,0.67,9.5,6
7.0,0.65,0.02,2.1,0.066,8.0,25.0,0.9972,3.47,0.67,9.5,6
9.4,0.615,0.28,3.2,0.087,18.0,72.0,1.0001,3.31,0.53,9.7,5
11.8,0.38,0.55,2.1,0.071,5.0,19.0,0.9986,3.11,0.62,10.8,6
10.6,1.02,0.43,2.9,0.076,26.0,88.0,0.9984,3.08,0.57,10.1,6
7.0,0.65,0.02,2.1,0.066,8.0,25.0,0.9972,3.47,0.67,9.5,6
7.0,0.64,0.02,2.1,0.067,9.0,23.0,0.997,3.47,0.67,9.4,6
7.5,0.38,0.48,2.6,0.073,22.0,84.0,0.9972,3.32,0.7,9.6,4
9.1,0.765,0.04,1.6,0.078,4.0,14.0,0.998,3.29,0.54,9.7,4
8.4,1.035,0.15,6.0,0.073,11.0,54.0,0.9990000000000001,3.37,0.49,9.9,5
7.0,0.78,0.08,2.0,0.09300000000000001,10.0,19.0,0.9956,3.4,0.47,10.0,5
7.4,0.49,0.19,3.0,0.077,16.0,37.0,0.9966,3.37,0.51,10.5,5
7.8,0.545,0.12,2.5,0.068,11.0,35.0,0.996,3.34,0.61,11.6,6
9.7,0.31,0.47,1.6,0.062,13.0,33.0,0.9983,3.27,0.66,10.0,6
10.6,1.025,0.43,2.8,0.08,21.0,84.0,0.9985,3.06,0.57,10.1,5
8.9,0.565,0.34,3.0,0.09300000000000001,16.0,112.0,0.9998,3.38,0.61,9.5,5
8.7,0.69,0.0,3.2,0.084,13.0,33.0,0.9992,3.36,0.45,9.4,5
8.0,0.43,0.36,2.3,0.075,10.0,48.0,0.9976,3.34,0.46,9.4,5
9.9,0.74,0.28,2.6,0.078,21.0,77.0,0.998,3.28,0.51,9.8,5
7.2,0.49,0.18,2.7,0.069,13.0,34.0,0.9967,3.29,0.48,9.2,6
8.0,0.43,0.36,2.3,0.075,10.0,48.0,0.9976,3.34,0.46,9.4,5
7.6,0.46,0.11,2.6,0.079,12.0,49.0,0.9968,3.21,0.57,10.0,5
8.4,0.56,0.04,2.0,0.08199999999999999,10.0,22.0,0.9976,3.22,0.44,9.6,5
7.1,0.66,0.0,3.9,0.086,17.0,45.0,0.9976,3.46,0.54,9.5,5
8.4,0.56,0.04,2.0,0.08199999999999999,10.0,22.0,0.9976,3.22,0.44,9.6,5
8.9,0.48,0.24,2.85,0.094,35.0,106.0,0.9982,3.1,0.53,9.2,5
7.6,0.42,0.08,2.7,0.084,15.0,48.0,0.9968,3.21,0.59,10.0,5
7.1,0.31,0.3,2.2,0.053,36.0,127.0,0.9965,2.94,1.62,9.5,5
7.5,1.115,0.1,3.1,0.086,5.0,12.0,0.9958,3.54,0.6,11.2,4
9.0,0.66,0.17,3.0,0.077,5.0,13.0,0.9976,3.29,0.55,10.4,5
8.1,0.72,0.09,2.8,0.084,18.0,49.0,0.9994,3.43,0.72,11.1,6
6.4,0.57,0.02,1.8,0.067,4.0,11.0,0.997,3.46,0.68,9.5,5
6.4,0.57,0.02,1.8,0.067,4.0,11.0,0.997,3.46,0.68,9.5,5
6.4,0.865,0.03,3.2,0.071,27.0,58.0,0.995,3.61,0.49,12.7,6
9.5,0.55,0.66,2.3,0.387,12.0,37.0,0.9982,3.17,0.67,9.6,5
8.9,0.875,0.13,3.45,0.08800000000000001,4.0,14.0,0.9994,3.44,0.52,11.5,5
7.3,0.835,0.03,2.1,0.092,10.0,19.0,0.9966,3.39,0.47,9.6,5
7.0,0.45,0.34,2.7,0.08199999999999999,16.0,72.0,0.998,3.55,0.6,9.5,5
7.7,0.56,0.2,2.0,0.075,9.0,39.0,0.9987,3.48,0.62,9.3,5
7.7,0.965,0.1,2.1,0.11199999999999999,11.0,22.0,0.9963,3.26,0.5,9.5,5
7.7,0.965,0.1,2.1,0.11199999999999999,11.0,22.0,0.9963,3.26,0.5,9.5,5
8.2,0.59,0.0,2.5,0.09300000000000001,19.0,58.0,1.0002,3.5,0.65,9.3,6
9.0,0.46,0.23,2.8,0.092,28.0,104.0,0.9983,3.1,0.56,9.2,5
9.0,0.69,0.0,2.4,0.08800000000000001,19.0,38.0,0.9990000000000001,3.35,0.6,9.3,5
8.3,0.76,0.29,4.2,0.075,12.0,16.0,0.9965,3.45,0.68,11.5,6
9.2,0.53,0.24,2.6,0.078,28.0,139.0,0.9978799999999999,3.21,0.57,9.5,5
6.5,0.615,0.0,1.9,0.065,9.0,18.0,0.9972,3.46,0.65,9.2,5
11.6,0.41,0.58,2.8,0.096,25.0,101.0,1.00024,3.13,0.53,10.0,5
11.1,0.39,0.54,2.7,0.095,21.0,101.0,1.0001,3.13,0.51,9.5,5
7.3,0.51,0.18,2.1,0.07,12.0,28.0,0.9976799999999999,3.52,0.73,9.5,6
8.2,0.34,0.38,2.5,0.08,12.0,57.0,0.9978,3.3,0.47,9.0,6
8.6,0.33,0.4,2.6,0.083,16.0,68.0,0.99782,3.3,0.48,9.4,5
7.2,0.5,0.18,2.1,0.071,12.0,31.0,0.99761,3.52,0.72,9.6,6
7.3,0.51,0.18,2.1,0.07,12.0,28.0,0.9976799999999999,3.52,0.73,9.5,6
8.3,0.65,0.1,2.9,0.08900000000000001,17.0,40.0,0.99803,3.29,0.55,9.5,5
8.3,0.65,0.1,2.9,0.08900000000000001,17.0,40.0,0.99803,3.29,0.55,9.5,5
7.6,0.54,0.13,2.5,0.09699999999999999,24.0,66.0,0.99785,3.39,0.61,9.4,5
8.3,0.65,0.1,2.9,0.08900000000000001,17.0,40.0,0.99803,3.29,0.55,9.5,5
7.8,0.48,0.68,1.7,0.415,14.0,32.0,0.99656,3.09,1.06,9.1,6
7.8,0.91,0.07,1.9,0.057999999999999996,22.0,47.0,0.99525,3.51,0.43,10.7,6
6.3,0.98,0.01,2.0,0.057,15.0,33.0,0.9948799999999999,3.6,0.46,11.2,6
8.1,0.87,0.0,2.2,0.084,10.0,31.0,0.99656,3.25,0.5,9.8,5
8.1,0.87,0.0,2.2,0.084,10.0,31.0,0.99656,3.25,0.5,9.8,5
8.8,0.42,0.21,2.5,0.092,33.0,88.0,0.99823,3.19,0.52,9.2,5
9.0,0.58,0.25,2.8,0.075,9.0,104.0,0.99779,3.23,0.57,9.7,5
9.3,0.655,0.26,2.0,0.096,5.0,35.0,0.9973799999999999,3.25,0.42,9.6,5
8.8,0.7,0.0,1.7,0.069,8.0,19.0,0.9970100000000001,3.31,0.53,10.0,6
9.3,0.655,0.26,2.0,0.096,5.0,35.0,0.9973799999999999,3.25,0.42,9.6,5
9.1,0.68,0.11,2.8,0.09300000000000001,11.0,44.0,0.9988799999999999,3.31,0.55,9.5,6
9.2,0.67,0.1,3.0,0.091,12.0,48.0,0.9988799999999999,3.31,0.54,9.5,6
8.8,0.59,0.18,2.9,0.08900000000000001,12.0,74.0,0.9973799999999999,3.14,0.54,9.4,5
7.5,0.6,0.32,2.7,0.10300000000000001,13.0,98.0,0.9993799999999999,3.45,0.62,9.5,5
7.1,0.59,0.02,2.3,0.08199999999999999,24.0,94.0,0.99744,3.55,0.53,9.7,6
7.9,0.72,0.01,1.9,0.076,7.0,32.0,0.9966799999999999,3.39,0.54,9.6,5
7.1,0.59,0.02,2.3,0.08199999999999999,24.0,94.0,0.99744,3.55,0.53,9.7,6
9.4,0.685,0.26,2.4,0.08199999999999999,23.0,143.0,0.9978,3.28,0.55,9.4,5
9.5,0.57,0.27,2.3,0.08199999999999999,23.0,144.0,0.99782,3.27,0.55,9.4,5
7.9,0.4,0.29,1.8,0.157,1.0,44.0,0.9973,3.3,0.92,9.5,6
7.9,0.4,0.3,1.8,0.157,2.0,45.0,0.9972700000000001,3.31,0.91,9.5,6
7.2,1.0,0.0,3.0,0.102,7.0,16.0,0.9958600000000001,3.43,0.46,10.0,5
6.9,0.765,0.18,2.4,0.243,5.5,48.0,0.9961200000000001,3.4,0.6,10.3,6
6.9,0.635,0.17,2.4,0.24100000000000002,6.0,18.0,0.9961,3.4,0.59,10.3,6
8.3,0.43,0.3,3.4,0.079,7.0,34.0,0.9978799999999999,3.36,0.61,10.5,5
7.1,0.52,0.03,2.6,0.076,21.0,92.0,0.99745,3.5,0.6,9.8,5
7.0,0.57,0.0,2.0,0.19,12.0,45.0,0.9967600000000001,3.31,0.6,9.4,6
6.5,0.46,0.14,2.4,0.114,9.0,37.0,0.9973200000000001,3.66,0.65,9.8,5
9.0,0.82,0.05,2.4,0.081,26.0,96.0,0.9981399999999999,3.36,0.53,10.0,5
6.5,0.46,0.14,2.4,0.114,9.0,37.0,0.9973200000000001,3.66,0.65,9.8,5
7.1,0.59,0.01,2.5,0.077,20.0,85.0,0.99746,3.55,0.59,9.8,5
9.9,0.35,0.41,2.3,0.083,11.0,61.0,0.9982,3.21,0.5,9.5,5
9.9,0.35,0.41,2.3,0.083,11.0,61.0,0.9982,3.21,0.5,9.5,5
10.0,0.56,0.24,2.2,0.079,19.0,58.0,0.9991,3.18,0.56,10.1,6
10.0,0.56,0.24,2.2,0.079,19.0,58.0,0.9991,3.18,0.56,10.1,6
8.6,0.63,0.17,2.9,0.099,21.0,119.0,0.998,3.09,0.52,9.3,5
7.4,0.37,0.43,2.6,0.08199999999999999,18.0,82.0,0.99708,3.33,0.68,9.7,6
8.8,0.64,0.17,2.9,0.084,25.0,130.0,0.99818,3.23,0.54,9.6,5
7.1,0.61,0.02,2.5,0.081,17.0,87.0,0.99745,3.48,0.6,9.7,6
7.7,0.6,0.0,2.6,0.055,7.0,13.0,0.99639,3.38,0.56,10.8,5
10.1,0.27,0.54,2.3,0.065,7.0,26.0,0.99531,3.17,0.53,12.5,6
10.8,0.89,0.3,2.6,0.132,7.0,60.0,0.9978600000000001,2.99,1.18,10.2,5
8.7,0.46,0.31,2.5,0.126,24.0,64.0,0.99746,3.1,0.74,9.6,5
9.3,0.37,0.44,1.6,0.038,21.0,42.0,0.99526,3.24,0.81,10.8,7
9.4,0.5,0.34,3.6,0.08199999999999999,5.0,14.0,0.9987,3.29,0.52,10.7,6
9.4,0.5,0.34,3.6,0.08199999999999999,5.0,14.0,0.9987,3.29,0.52,10.7,6
7.2,0.61,0.08,4.0,0.08199999999999999,26.0,108.0,0.99641,3.25,0.51,9.4,5
8.6,0.55,0.09,3.3,0.068,8.0,17.0,0.99735,3.23,0.44,10.0,5
5.1,0.585,0.0,1.7,0.044000000000000004,14.0,86.0,0.99264,3.56,0.94,12.9,7
7.7,0.56,0.08,2.5,0.114,14.0,46.0,0.9971,3.24,0.66,9.6,6
8.4,0.52,0.22,2.7,0.084,4.0,18.0,0.99682,3.26,0.57,9.9,6
8.2,0.28,0.4,2.4,0.052000000000000005,4.0,10.0,0.99356,3.33,0.7,12.8,7
8.4,0.25,0.39,2.0,0.040999999999999995,4.0,10.0,0.9938600000000001,3.27,0.71,12.5,7
8.2,0.28,0.4,2.4,0.052000000000000005,4.0,10.0,0.99356,3.33,0.7,12.8,7
7.4,0.53,0.12,1.9,0.165,4.0,12.0,0.99702,3.26,0.86,9.2,5
7.6,0.48,0.31,2.8,0.07,4.0,15.0,0.9969299999999999,3.22,0.55,10.3,6
7.3,0.49,0.1,2.6,0.068,4.0,14.0,0.9956200000000001,3.3,0.47,10.5,5
12.9,0.5,0.55,2.8,0.07200000000000001,7.0,24.0,1.0001200000000001,3.09,0.68,10.9,6
10.8,0.45,0.33,2.5,0.099,20.0,38.0,0.99818,3.24,0.71,10.8,5
6.9,0.39,0.24,2.1,0.102,4.0,7.0,0.9946200000000001,3.44,0.58,11.4,4
12.6,0.41,0.54,2.8,0.10300000000000001,19.0,41.0,0.99939,3.21,0.76,11.3,6
10.8,0.45,0.33,2.5,0.099,20.0,38.0,0.99818,3.24,0.71,10.8,5
9.8,0.51,0.19,3.2,0.081,8.0,30.0,0.9984,3.23,0.58,10.5,6
10.8,0.29,0.42,1.6,0.084,19.0,27.0,0.99545,3.28,0.73,11.9,6
7.1,0.715,0.0,2.35,0.071,21.0,47.0,0.9963200000000001,3.29,0.45,9.4,5
9.1,0.66,0.15,3.2,0.09699999999999999,9.0,59.0,0.99976,3.28,0.54,9.6,5
7.0,0.685,0.0,1.9,0.099,9.0,22.0,0.9960600000000001,3.34,0.6,9.7,5
4.9,0.42,0.0,2.1,0.048,16.0,42.0,0.99154,3.71,0.74,14.0,7
6.7,0.54,0.13,2.0,0.076,15.0,36.0,0.9973,3.61,0.64,9.8,5
6.7,0.54,0.13,2.0,0.076,15.0,36.0,0.9973,3.61,0.64,9.8,5
7.1,0.48,0.28,2.8,0.068,6.0,16.0,0.99682,3.24,0.53,10.3,5
7.1,0.46,0.14,2.8,0.076,15.0,37.0,0.99624,3.36,0.49,10.7,5
7.5,0.27,0.34,2.3,0.05,4.0,8.0,0.9951,3.4,0.64,11.0,7
7.1,0.46,0.14,2.8,0.076,15.0,37.0,0.99624,3.36,0.49,10.7,5
7.8,0.57,0.09,2.3,0.065,34.0,45.0,0.9941700000000001,3.46,0.74,12.7,8
5.9,0.61,0.08,2.1,0.071,16.0,24.0,0.9937600000000001,3.56,0.77,11.1,6
7.5,0.685,0.07,2.5,0.057999999999999996,5.0,9.0,0.9963200000000001,3.38,0.55,10.9,4
5.9,0.61,0.08,2.1,0.071,16.0,24.0,0.9937600000000001,3.56,0.77,11.1,6
10.4,0.44,0.42,1.5,0.145,34.0,48.0,0.9983200000000001,3.38,0.86,9.9,3
11.6,0.47,0.44,1.6,0.147,36.0,51.0,0.99836,3.38,0.86,9.9,4
8.8,0.685,0.26,1.6,0.08800000000000001,16.0,23.0,0.9969399999999999,3.32,0.47,9.4,5
7.6,0.665,0.1,1.5,0.066,27.0,55.0,0.99655,3.39,0.51,9.3,5
6.7,0.28,0.28,2.4,0.012,36.0,100.0,0.99064,3.26,0.39,11.7,7
6.7,0.28,0.28,2.4,0.012,36.0,100.0,0.99064,3.26,0.39,11.7,7
10.1,0.31,0.35,1.6,0.075,9.0,28.0,0.99672,3.24,0.83,11.2,7
6.0,0.5,0.04,2.2,0.092,13.0,26.0,0.9964700000000001,3.46,0.47,10.0,5
11.1,0.42,0.47,2.65,0.085,9.0,34.0,0.99736,3.24,0.77,12.1,7
6.6,0.66,0.0,3.0,0.115,21.0,31.0,0.99629,3.45,0.63,10.3,5
10.6,0.5,0.45,2.6,0.11900000000000001,34.0,68.0,0.99708,3.23,0.72,10.9,6
7.1,0.685,0.35,2.0,0.08800000000000001,9.0,92.0,0.9963,3.28,0.62,9.4,5
9.9,0.25,0.46,1.7,0.062,26.0,42.0,0.9959,3.18,0.83,10.6,6
6.4,0.64,0.21,1.8,0.081,14.0,31.0,0.9968899999999999,3.59,0.66,9.8,5
6.4,0.64,0.21,1.8,0.081,14.0,31.0,0.9968899999999999,3.59,0.66,9.8,5
7.4,0.68,0.16,1.8,0.078,12.0,39.0,0.9977,3.5,0.7,9.9,6
6.4,0.64,0.21,1.8,0.081,14.0,31.0,0.9968899999999999,3.59,0.66,9.8,5
6.4,0.63,0.21,1.6,0.08,12.0,32.0,0.9968899999999999,3.58,0.66,9.8,5
9.3,0.43,0.44,1.9,0.085,9.0,22.0,0.99708,3.28,0.55,9.5,5
9.3,0.43,0.44,1.9,0.085,9.0,22.0,0.99708,3.28,0.55,9.5,5
8.0,0.42,0.32,2.5,0.08,26.0,122.0,0.9980100000000001,3.22,1.07,9.7,5
9.3,0.36,0.39,1.5,0.08,41.0,55.0,0.9965200000000001,3.47,0.73,10.9,6
9.3,0.36,0.39,1.5,0.08,41.0,55.0,0.9965200000000001,3.47,0.73,10.9,6
7.6,0.735,0.02,2.5,0.071,10.0,14.0,0.9953799999999999,3.51,0.71,11.7,7
9.3,0.36,0.39,1.5,0.08,41.0,55.0,0.9965200000000001,3.47,0.73,10.9,6
8.2,0.26,0.34,2.5,0.073,16.0,47.0,0.9959399999999999,3.4,0.78,11.3,7
11.7,0.28,0.47,1.7,0.054000000000000006,17.0,32.0,0.9968600000000001,3.15,0.67,10.6,7
6.8,0.56,0.22,1.8,0.07400000000000001,15.0,24.0,0.9943799999999999,3.4,0.82,11.2,6
7.2,0.62,0.06,2.7,0.077,15.0,85.0,0.99746,3.51,0.54,9.5,5
5.8,1.01,0.66,2.0,0.039,15.0,88.0,0.9935700000000001,3.66,0.6,11.5,6
7.5,0.42,0.32,2.7,0.067,7.0,25.0,0.9962799999999999,3.24,0.44,10.4,5
7.2,0.62,0.06,2.5,0.078,17.0,84.0,0.99746,3.51,0.53,9.7,5
7.2,0.62,0.06,2.7,0.077,15.0,85.0,0.99746,3.51,0.54,9.5,5
7.2,0.635,0.07,2.6,0.077,16.0,86.0,0.9974799999999999,3.51,0.54,9.7,5
6.8,0.49,0.22,2.3,0.071,13.0,24.0,0.9943799999999999,3.41,0.83,11.3,6
6.9,0.51,0.23,2.0,0.07200000000000001,13.0,22.0,0.9943799999999999,3.4,0.84,11.2,6
6.8,0.56,0.22,1.8,0.07400000000000001,15.0,24.0,0.9943799999999999,3.4,0.82,11.2,6
7.6,0.63,0.03,2.0,0.08,27.0,43.0,0.9957799999999999,3.44,0.64,10.9,6
7.7,0.715,0.01,2.1,0.064,31.0,43.0,0.99371,3.41,0.57,11.8,6
6.9,0.56,0.03,1.5,0.086,36.0,46.0,0.9952200000000001,3.53,0.57,10.6,5
7.3,0.35,0.24,2.0,0.067,28.0,48.0,0.9957600000000001,3.43,0.54,10.0,4
9.1,0.21,0.37,1.6,0.067,6.0,10.0,0.9955200000000001,3.23,0.58,11.1,7
10.4,0.38,0.46,2.1,0.10400000000000001,6.0,10.0,0.99664,3.12,0.65,11.8,7
8.8,0.31,0.4,2.8,0.109,7.0,16.0,0.9961399999999999,3.31,0.79,11.8,7
7.1,0.47,0.0,2.2,0.067,7.0,14.0,0.9951700000000001,3.4,0.58,10.9,4
7.7,0.715,0.01,2.1,0.064,31.0,43.0,0.99371,3.41,0.57,11.8,6
8.8,0.61,0.19,4.0,0.094,30.0,69.0,0.99787,3.22,0.5,10.0,6
7.2,0.6,0.04,2.5,0.076,18.0,88.0,0.99745,3.53,0.55,9.5,5
9.2,0.56,0.18,1.6,0.078,10.0,21.0,0.9957600000000001,3.15,0.49,9.9,5
7.6,0.715,0.0,2.1,0.068,30.0,35.0,0.9953299999999999,3.48,0.65,11.4,6
8.4,0.31,0.29,3.1,0.19399999999999998,14.0,26.0,0.99536,3.22,0.78,12.0,6
7.2,0.6,0.04,2.5,0.076,18.0,88.0,0.99745,3.53,0.55,9.5,5
8.8,0.61,0.19,4.0,0.094,30.0,69.0,0.99787,3.22,0.5,10.0,6
8.9,0.75,0.14,2.5,0.086,9.0,30.0,0.99824,3.34,0.64,10.5,5
9.0,0.8,0.12,2.4,0.083,8.0,28.0,0.99836,3.33,0.65,10.4,6
10.7,0.52,0.38,2.6,0.066,29.0,56.0,0.99577,3.15,0.79,12.1,7
6.8,0.57,0.0,2.5,0.07200000000000001,32.0,64.0,0.9949100000000001,3.43,0.56,11.2,6
10.7,0.9,0.34,6.6,0.11199999999999999,23.0,99.0,1.00289,3.22,0.68,9.3,5
7.2,0.34,0.24,2.0,0.071,30.0,52.0,0.9957600000000001,3.44,0.58,10.1,5
7.2,0.66,0.03,2.3,0.078,16.0,86.0,0.9974299999999999,3.53,0.57,9.7,5
10.1,0.45,0.23,1.9,0.08199999999999999,10.0,18.0,0.99774,3.22,0.65,9.3,6
7.2,0.66,0.03,2.3,0.078,16.0,86.0,0.9974299999999999,3.53,0.57,9.7,5
7.2,0.63,0.03,2.2,0.08,17.0,88.0,0.99745,3.53,0.58,9.8,6
7.1,0.59,0.01,2.3,0.08,27.0,43.0,0.9955,3.42,0.58,10.7,6
8.3,0.31,0.39,2.4,0.078,17.0,43.0,0.99444,3.31,0.77,12.5,7
7.1,0.59,0.01,2.3,0.08,27.0,43.0,0.9955,3.42,0.58,10.7,6
8.3,0.31,0.39,2.4,0.078,17.0,43.0,0.99444,3.31,0.77,12.5,7
8.3,1.02,0.02,3.4,0.084,6.0,11.0,0.99892,3.48,0.49,11.0,3
8.9,0.31,0.36,2.6,0.055999999999999994,10.0,39.0,0.9956200000000001,3.4,0.69,11.8,5
7.4,0.635,0.1,2.4,0.08,16.0,33.0,0.99736,3.58,0.69,10.8,7
7.4,0.635,0.1,2.4,0.08,16.0,33.0,0.99736,3.58,0.69,10.8,7
6.8,0.59,0.06,6.0,0.06,11.0,18.0,0.9962,3.41,0.59,10.8,7
6.8,0.59,0.06,6.0,0.06,11.0,18.0,0.9962,3.41,0.59,10.8,7
9.2,0.58,0.2,3.0,0.081,15.0,115.0,0.998,3.23,0.59,9.5,5
7.2,0.54,0.27,2.6,0.084,12.0,78.0,0.9964,3.39,0.71,11.0,5
6.1,0.56,0.0,2.2,0.079,6.0,9.0,0.9948,3.59,0.54,11.5,6
7.4,0.52,0.13,2.4,0.078,34.0,61.0,0.9952799999999999,3.43,0.59,10.8,6
7.3,0.305,0.39,1.2,0.059000000000000004,7.0,11.0,0.99331,3.29,0.52,11.5,6
9.3,0.38,0.48,3.8,0.132,3.0,11.0,0.99577,3.23,0.57,13.2,6
9.1,0.28,0.46,9.0,0.114,3.0,9.0,0.9990100000000001,3.18,0.6,10.9,6
10.0,0.46,0.44,2.9,0.065,4.0,8.0,0.99674,3.33,0.62,12.2,6
9.4,0.395,0.46,4.6,0.094,3.0,10.0,0.99639,3.27,0.64,12.2,7
7.3,0.305,0.39,1.2,0.059000000000000004,7.0,11.0,0.99331,3.29,0.52,11.5,6
8.6,0.315,0.4,2.2,0.079,3.0,6.0,0.9951200000000001,3.27,0.67,11.9,6
5.3,0.715,0.19,1.5,0.161,7.0,62.0,0.99395,3.62,0.61,11.0,5
6.8,0.41,0.31,8.8,0.084,26.0,45.0,0.99824,3.38,0.64,10.1,6
8.4,0.36,0.32,2.2,0.081,32.0,79.0,0.9964,3.3,0.72,11.0,6
8.4,0.62,0.12,1.8,0.07200000000000001,38.0,46.0,0.9950399999999999,3.38,0.89,11.8,6
9.6,0.41,0.37,2.3,0.091,10.0,23.0,0.9978600000000001,3.24,0.56,10.5,5
8.4,0.36,0.32,2.2,0.081,32.0,79.0,0.9964,3.3,0.72,11.0,6
8.4,0.62,0.12,1.8,0.07200000000000001,38.0,46.0,0.9950399999999999,3.38,0.89,11.8,6
6.8,0.41,0.31,8.8,0.084,26.0,45.0,0.99824,3.38,0.64,10.1,6
8.6,0.47,0.27,2.3,0.055,14.0,28.0,0.99516,3.18,0.8,11.2,5
8.6,0.22,0.36,1.9,0.064,53.0,77.0,0.9960399999999999,3.47,0.87,11.0,7
9.4,0.24,0.33,2.3,0.061,52.0,73.0,0.9978600000000001,3.47,0.9,10.2,6
8.4,0.67,0.19,2.2,0.09300000000000001,11.0,75.0,0.99736,3.2,0.59,9.2,4
8.6,0.47,0.27,2.3,0.055,14.0,28.0,0.99516,3.18,0.8,11.2,5
8.7,0.33,0.38,3.3,0.063,10.0,19.0,0.9946799999999999,3.3,0.73,12.0,7
6.6,0.61,0.01,1.9,0.08,8.0,25.0,0.99746,3.69,0.73,10.5,5
7.4,0.61,0.01,2.0,0.07400000000000001,13.0,38.0,0.9974799999999999,3.48,0.65,9.8,5
7.6,0.4,0.29,1.9,0.078,29.0,66.0,0.9971,3.45,0.59,9.5,6
7.4,0.61,0.01,2.0,0.07400000000000001,13.0,38.0,0.9974799999999999,3.48,0.65,9.8,5
6.6,0.61,0.01,1.9,0.08,8.0,25.0,0.99746,3.69,0.73,10.5,5
8.8,0.3,0.38,2.3,0.06,19.0,72.0,0.9954299999999999,3.39,0.72,11.8,6
8.8,0.3,0.38,2.3,0.06,19.0,72.0,0.9954299999999999,3.39,0.72,11.8,6
12.0,0.63,0.5,1.4,0.071,6.0,26.0,0.9979100000000001,3.07,0.6,10.4,4
7.2,0.38,0.38,2.8,0.068,23.0,42.0,0.99356,3.34,0.72,12.9,7
6.2,0.46,0.17,1.6,0.073,7.0,11.0,0.99425,3.61,0.54,11.4,5
9.6,0.33,0.52,2.2,0.07400000000000001,13.0,25.0,0.9950899999999999,3.36,0.76,12.4,7
9.9,0.27,0.49,5.0,0.08199999999999999,9.0,17.0,0.99484,3.19,0.52,12.5,7
10.1,0.43,0.4,2.6,0.092,13.0,52.0,0.99834,3.22,0.64,10.0,7
9.8,0.5,0.34,2.3,0.094,10.0,45.0,0.99864,3.24,0.6,9.7,7
8.3,0.3,0.49,3.8,0.09,11.0,24.0,0.99498,3.27,0.64,12.1,7
10.2,0.44,0.42,2.0,0.071,7.0,20.0,0.99566,3.14,0.79,11.1,7
10.2,0.44,0.58,4.1,0.092,11.0,24.0,0.99745,3.29,0.99,12.0,7
8.3,0.28,0.48,2.1,0.09300000000000001,6.0,12.0,0.99408,3.26,0.62,12.4,7
8.9,0.12,0.45,1.8,0.075,10.0,21.0,0.9955200000000001,3.41,0.76,11.9,7
8.9,0.12,0.45,1.8,0.075,10.0,21.0,0.9955200000000001,3.41,0.76,11.9,7
8.9,0.12,0.45,1.8,0.075,10.0,21.0,0.9955200000000001,3.41,0.76,11.9,7
8.3,0.28,0.48,2.1,0.09300000000000001,6.0,12.0,0.99408,3.26,0.62,12.4,7
8.2,0.31,0.4,2.2,0.057999999999999996,6.0,10.0,0.99536,3.31,0.68,11.2,7
10.2,0.34,0.48,2.1,0.052000000000000005,5.0,9.0,0.9945799999999999,3.2,0.69,12.1,7
7.6,0.43,0.4,2.7,0.08199999999999999,6.0,11.0,0.9953799999999999,3.44,0.54,12.2,6
8.5,0.21,0.52,1.9,0.09,9.0,23.0,0.9964799999999999,3.36,0.67,10.4,5
9.0,0.36,0.52,2.1,0.111,5.0,10.0,0.9956799999999999,3.31,0.62,11.3,6
9.5,0.37,0.52,2.0,0.08800000000000001,12.0,51.0,0.99613,3.29,0.58,11.1,6
6.4,0.57,0.12,2.3,0.12,25.0,36.0,0.99519,3.47,0.71,11.3,7
8.0,0.59,0.05,2.0,0.08900000000000001,12.0,32.0,0.99735,3.36,0.61,10.0,5
8.5,0.47,0.27,1.9,0.057999999999999996,18.0,38.0,0.99518,3.16,0.85,11.1,6
7.1,0.56,0.14,1.6,0.078,7.0,18.0,0.99592,3.27,0.62,9.3,5
6.6,0.57,0.02,2.1,0.115,6.0,16.0,0.99654,3.38,0.69,9.5,5
8.8,0.27,0.39,2.0,0.1,20.0,27.0,0.99546,3.15,0.69,11.2,6
8.5,0.47,0.27,1.9,0.057999999999999996,18.0,38.0,0.99518,3.16,0.85,11.1,6
8.3,0.34,0.4,2.4,0.065,24.0,48.0,0.99554,3.34,0.86,11.0,6
9.0,0.38,0.41,2.4,0.10300000000000001,6.0,10.0,0.9960399999999999,3.13,0.58,11.9,7
8.5,0.66,0.2,2.1,0.09699999999999999,23.0,113.0,0.9973299999999999,3.13,0.48,9.2,5
9.0,0.4,0.43,2.4,0.068,29.0,46.0,0.9943,3.2,0.6,12.2,6
6.7,0.56,0.09,2.9,0.079,7.0,22.0,0.99669,3.46,0.61,10.2,5
10.4,0.26,0.48,1.9,0.066,6.0,10.0,0.99724,3.33,0.87,10.9,6
10.4,0.26,0.48,1.9,0.066,6.0,10.0,0.99724,3.33,0.87,10.9,6
10.1,0.38,0.5,2.4,0.10400000000000001,6.0,13.0,0.9964299999999999,3.22,0.65,11.6,7
8.5,0.34,0.44,1.7,0.079,6.0,12.0,0.99605,3.52,0.63,10.7,5
8.8,0.33,0.41,5.9,0.073,7.0,13.0,0.9965799999999999,3.3,0.62,12.1,7
7.2,0.41,0.3,2.1,0.083,35.0,72.0,0.997,3.44,0.52,9.4,5
7.2,0.41,0.3,2.1,0.083,35.0,72.0,0.997,3.44,0.52,9.4,5
8.4,0.59,0.29,2.6,0.109,31.0,119.0,0.9980100000000001,3.15,0.5,9.1,5
7.0,0.4,0.32,3.6,0.061,9.0,29.0,0.99416,3.28,0.49,11.3,7
12.2,0.45,0.49,1.4,0.075,3.0,6.0,0.9969,3.13,0.63,10.4,5
9.1,0.5,0.3,1.9,0.065,8.0,17.0,0.99774,3.32,0.71,10.5,6
9.5,0.86,0.26,1.9,0.079,13.0,28.0,0.9971200000000001,3.25,0.62,10.0,5
7.3,0.52,0.32,2.1,0.07,51.0,70.0,0.99418,3.34,0.82,12.9,6
9.1,0.5,0.3,1.9,0.065,8.0,17.0,0.99774,3.32,0.71,10.5,6
12.2,0.45,0.49,1.4,0.075,3.0,6.0,0.9969,3.13,0.63,10.4,5
7.4,0.58,0.0,2.0,0.064,7.0,11.0,0.9956200000000001,3.45,0.58,11.3,6
9.8,0.34,0.39,1.4,0.066,3.0,7.0,0.9947,3.19,0.55,11.4,7
7.1,0.36,0.3,1.6,0.08,35.0,70.0,0.9969299999999999,3.44,0.5,9.4,5
7.7,0.39,0.12,1.7,0.09699999999999999,19.0,27.0,0.9959600000000001,3.16,0.49,9.4,5
9.7,0.295,0.4,1.5,0.073,14.0,21.0,0.99556,3.14,0.51,10.9,6
7.7,0.39,0.12,1.7,0.09699999999999999,19.0,27.0,0.9959600000000001,3.16,0.49,9.4,5
7.1,0.34,0.28,2.0,0.08199999999999999,31.0,68.0,0.9969399999999999,3.45,0.48,9.4,5
6.5,0.4,0.1,2.0,0.076,30.0,47.0,0.99554,3.36,0.48,9.4,6
7.1,0.34,0.28,2.0,0.08199999999999999,31.0,68.0,0.9969399999999999,3.45,0.48,9.4,5
10.0,0.35,0.45,2.5,0.092,20.0,88.0,0.99918,3.15,0.43,9.4,5
7.7,0.6,0.06,2.0,0.079,19.0,41.0,0.99697,3.39,0.62,10.1,6
5.6,0.66,0.0,2.2,0.087,3.0,11.0,0.9937799999999999,3.71,0.63,12.8,7
5.6,0.66,0.0,2.2,0.087,3.0,11.0,0.9937799999999999,3.71,0.63,12.8,7
8.9,0.84,0.34,1.4,0.05,4.0,10.0,0.99554,3.12,0.48,9.1,6
6.4,0.69,0.0,1.65,0.055,7.0,12.0,0.9916200000000001,3.47,0.53,12.9,6
7.5,0.43,0.3,2.2,0.062,6.0,12.0,0.99495,3.44,0.72,11.5,7
9.9,0.35,0.38,1.5,0.057999999999999996,31.0,47.0,0.9967600000000001,3.26,0.82,10.6,7
9.1,0.29,0.33,2.05,0.063,13.0,27.0,0.99516,3.26,0.84,11.7,7
6.8,0.36,0.32,1.8,0.067,4.0,8.0,0.9928,3.36,0.55,12.8,7
8.2,0.43,0.29,1.6,0.081,27.0,45.0,0.99603,3.25,0.54,10.3,5
6.8,0.36,0.32,1.8,0.067,4.0,8.0,0.9928,3.36,0.55,12.8,7
9.1,0.29,0.33,2.05,0.063,13.0,27.0,0.99516,3.26,0.84,11.7,7
9.1,0.3,0.34,2.0,0.064,12.0,25.0,0.99516,3.26,0.84,11.7,7
8.9,0.35,0.4,3.6,0.11,12.0,24.0,0.99549,3.23,0.7,12.0,7
9.6,0.5,0.36,2.8,0.11599999999999999,26.0,55.0,0.9972200000000001,3.18,0.68,10.9,5
8.9,0.28,0.45,1.7,0.067,7.0,12.0,0.99354,3.25,0.55,12.3,7
8.9,0.32,0.31,2.0,0.08800000000000001,12.0,19.0,0.9957,3.17,0.55,10.4,6
7.7,1.005,0.15,2.1,0.102,11.0,32.0,0.9960399999999999,3.23,0.48,10.0,5
7.5,0.71,0.0,1.6,0.092,22.0,31.0,0.99635,3.38,0.58,10.0,6
8.0,0.58,0.16,2.0,0.12,3.0,7.0,0.99454,3.22,0.58,11.2,6
10.5,0.39,0.46,2.2,0.075,14.0,27.0,0.99598,3.06,0.84,11.4,6
8.9,0.38,0.4,2.2,0.068,12.0,28.0,0.9948600000000001,3.27,0.75,12.6,7
8.0,0.18,0.37,0.9,0.049,36.0,109.0,0.9900700000000001,2.89,0.44,12.7,6
8.0,0.18,0.37,0.9,0.049,36.0,109.0,0.9900700000000001,2.89,0.44,12.7,6
7.0,0.5,0.14,1.8,0.078,10.0,23.0,0.99636,3.53,0.61,10.4,5
11.3,0.36,0.66,2.4,0.12300000000000001,3.0,8.0,0.9964200000000001,3.2,0.53,11.9,6
11.3,0.36,0.66,2.4,0.12300000000000001,3.0,8.0,0.9964200000000001,3.2,0.53,11.9,6
7.0,0.51,0.09,2.1,0.062,4.0,9.0,0.99584,3.35,0.54,10.5,5
8.2,0.32,0.42,2.3,0.098,3.0,9.0,0.9950600000000001,3.27,0.55,12.3,6
7.7,0.58,0.01,1.8,0.08800000000000001,12.0,18.0,0.9956799999999999,3.32,0.56,10.5,7
8.6,0.83,0.0,2.8,0.095,17.0,43.0,0.9982200000000001,3.33,0.6,10.4,6
7.9,0.31,0.32,1.9,0.066,14.0,36.0,0.99364,3.41,0.56,12.6,6
6.4,0.795,0.0,2.2,0.065,28.0,52.0,0.9937799999999999,3.49,0.52,11.6,5
7.2,0.34,0.21,2.5,0.075,41.0,68.0,0.9958600000000001,3.37,0.54,10.1,6
7.7,0.58,0.01,1.8,0.08800000000000001,12.0,18.0,0.9956799999999999,3.32,0.56,10.5,7
7.1,0.59,0.0,2.1,0.091,9.0,14.0,0.9948799999999999,3.42,0.55,11.5,7
7.3,0.55,0.01,1.8,0.09300000000000001,9.0,15.0,0.9951399999999999,3.35,0.58,11.0,7
8.1,0.82,0.0,4.1,0.095,5.0,14.0,0.99854,3.36,0.53,9.6,5
7.5,0.57,0.08,2.6,0.08900000000000001,14.0,27.0,0.99592,3.3,0.59,10.4,6
8.9,0.745,0.18,2.5,0.077,15.0,48.0,0.99739,3.2,0.47,9.7,6
10.1,0.37,0.34,2.4,0.085,5.0,17.0,0.9968299999999999,3.17,0.65,10.6,7
7.6,0.31,0.34,2.5,0.08199999999999999,26.0,35.0,0.99356,3.22,0.59,12.5,7
7.3,0.91,0.1,1.8,0.07400000000000001,20.0,56.0,0.99672,3.35,0.56,9.2,5
8.7,0.41,0.41,6.2,0.078,25.0,42.0,0.9953,3.24,0.77,12.6,7
8.9,0.5,0.21,2.2,0.08800000000000001,21.0,39.0,0.99692,3.33,0.83,11.1,6
7.4,0.965,0.0,2.2,0.08800000000000001,16.0,32.0,0.99756,3.58,0.67,10.2,5
6.9,0.49,0.19,1.7,0.079,13.0,26.0,0.9954700000000001,3.38,0.64,9.8,6
8.9,0.5,0.21,2.2,0.08800000000000001,21.0,39.0,0.99692,3.33,0.83,11.1,6
9.5,0.39,0.41,8.9,0.069,18.0,39.0,0.99859,3.29,0.81,10.9,7
6.4,0.39,0.33,3.3,0.046,12.0,53.0,0.9929399999999999,3.36,0.62,12.2,6
6.9,0.44,0.0,1.4,0.07,32.0,38.0,0.9943799999999999,3.32,0.58,11.4,6
7.6,0.78,0.0,1.7,0.076,33.0,45.0,0.9961200000000001,3.31,0.62,10.7,6
7.1,0.43,0.17,1.8,0.08199999999999999,27.0,51.0,0.99634,3.49,0.64,10.4,5
9.3,0.49,0.36,1.7,0.081,3.0,14.0,0.99702,3.27,0.78,10.9,6
9.3,0.5,0.36,1.8,0.084,6.0,17.0,0.9970399999999999,3.27,0.77,10.8,6
7.1,0.43,0.17,1.8,0.08199999999999999,27.0,51.0,0.99634,3.49,0.64,10.4,5
8.5,0.46,0.59,1.4,0.414,16.0,45.0,0.99702,3.03,1.34,9.2,5
5.6,0.605,0.05,2.4,0.073,19.0,25.0,0.9925799999999999,3.56,0.55,12.9,5
8.3,0.33,0.42,2.3,0.07,9.0,20.0,0.99426,3.38,0.77,12.7,7
8.2,0.64,0.27,2.0,0.095,5.0,77.0,0.9974700000000001,3.13,0.62,9.1,6
8.2,0.64,0.27,2.0,0.095,5.0,77.0,0.9974700000000001,3.13,0.62,9.1,6
8.9,0.48,0.53,4.0,0.10099999999999999,3.0,10.0,0.9958600000000001,3.21,0.59,12.1,7
7.6,0.42,0.25,3.9,0.10400000000000001,28.0,90.0,0.99784,3.15,0.57,9.1,5
9.9,0.53,0.57,2.4,0.09300000000000001,30.0,52.0,0.9971,3.19,0.76,11.6,7
8.9,0.48,0.53,4.0,0.10099999999999999,3.0,10.0,0.9958600000000001,3.21,0.59,12.1,7
11.6,0.23,0.57,1.8,0.07400000000000001,3.0,8.0,0.9981,3.14,0.7,9.9,6
9.1,0.4,0.5,1.8,0.071,7.0,16.0,0.9946200000000001,3.21,0.69,12.5,8
8.0,0.38,0.44,1.9,0.098,6.0,15.0,0.9956,3.3,0.64,11.4,6
10.2,0.29,0.65,2.4,0.075,6.0,17.0,0.99565,3.22,0.63,11.8,6
8.2,0.74,0.09,2.0,0.067,5.0,10.0,0.99418,3.28,0.57,11.8,6
7.7,0.61,0.18,2.4,0.083,6.0,20.0,0.9963,3.29,0.6,10.2,6
6.6,0.52,0.08,2.4,0.07,13.0,26.0,0.9935799999999999,3.4,0.72,12.5,7
11.1,0.31,0.53,2.2,0.06,3.0,10.0,0.99572,3.02,0.83,10.9,7
11.1,0.31,0.53,2.2,0.06,3.0,10.0,0.99572,3.02,0.83,10.9,7
8.0,0.62,0.35,2.8,0.086,28.0,52.0,0.997,3.31,0.62,10.8,5
9.3,0.33,0.45,1.5,0.057,19.0,37.0,0.99498,3.18,0.89,11.1,7
7.5,0.77,0.2,8.1,0.098,30.0,92.0,0.99892,3.2,0.58,9.2,5
7.2,0.35,0.26,1.8,0.083,33.0,75.0,0.9968,3.4,0.58,9.5,6
8.0,0.62,0.33,2.7,0.08800000000000001,16.0,37.0,0.9972,3.31,0.58,10.7,6
7.5,0.77,0.2,8.1,0.098,30.0,92.0,0.99892,3.2,0.58,9.2,5
9.1,0.25,0.34,2.0,0.071,45.0,67.0,0.99769,3.44,0.86,10.2,7
9.9,0.32,0.56,2.0,0.073,3.0,8.0,0.99534,3.15,0.73,11.4,6
8.6,0.37,0.65,6.4,0.08,3.0,8.0,0.9981700000000001,3.27,0.58,11.0,5
8.6,0.37,0.65,6.4,0.08,3.0,8.0,0.9981700000000001,3.27,0.58,11.0,5
7.9,0.3,0.68,8.3,0.05,37.5,278.0,0.99316,3.01,0.51,12.3,7
10.3,0.27,0.56,1.4,0.047,3.0,8.0,0.99471,3.16,0.51,11.8,6
7.9,0.3,0.68,8.3,0.05,37.5,289.0,0.99316,3.01,0.51,12.3,7
7.2,0.38,0.3,1.8,0.073,31.0,70.0,0.99685,3.42,0.59,9.5,6
8.7,0.42,0.45,2.4,0.07200000000000001,32.0,59.0,0.9961700000000001,3.33,0.77,12.0,6
7.2,0.38,0.3,1.8,0.073,31.0,70.0,0.99685,3.42,0.59,9.5,6
6.8,0.48,0.08,1.8,0.07400000000000001,40.0,64.0,0.99529,3.12,0.49,9.6,5
8.5,0.34,0.4,4.7,0.055,3.0,9.0,0.9973799999999999,3.38,0.66,11.6,7
7.9,0.19,0.42,1.6,0.057,18.0,30.0,0.9940000000000001,3.29,0.69,11.2,6
11.6,0.41,0.54,1.5,0.095,22.0,41.0,0.99735,3.02,0.76,9.9,7
11.6,0.41,0.54,1.5,0.095,22.0,41.0,0.99735,3.02,0.76,9.9,7
10.0,0.26,0.54,1.9,0.083,42.0,74.0,0.99451,2.98,0.63,11.8,8
7.9,0.34,0.42,2.0,0.086,8.0,19.0,0.99546,3.35,0.6,11.4,6
7.0,0.54,0.09,2.0,0.081,10.0,16.0,0.99479,3.43,0.59,11.5,6
9.2,0.31,0.36,2.2,0.079,11.0,31.0,0.99615,3.33,0.86,12.0,7
6.6,0.725,0.09,5.5,0.11699999999999999,9.0,17.0,0.99655,3.35,0.49,10.8,6
9.4,0.4,0.47,2.5,0.087,6.0,20.0,0.99772,3.15,0.5,10.5,5
6.6,0.725,0.09,5.5,0.11699999999999999,9.0,17.0,0.99655,3.35,0.49,10.8,6
8.6,0.52,0.38,1.5,0.096,5.0,18.0,0.99666,3.2,0.52,9.4,5
8.0,0.31,0.45,2.1,0.21600000000000003,5.0,16.0,0.9935799999999999,3.15,0.81,12.5,7
8.6,0.52,0.38,1.5,0.096,5.0,18.0,0.99666,3.2,0.52,9.4,5
8.4,0.34,0.42,2.1,0.07200000000000001,23.0,36.0,0.99392,3.11,0.78,12.4,6
7.4,0.49,0.27,2.1,0.071,14.0,25.0,0.9938799999999999,3.35,0.63,12.0,6
6.1,0.48,0.09,1.7,0.078,18.0,30.0,0.9940200000000001,3.45,0.54,11.2,6
7.4,0.49,0.27,2.1,0.071,14.0,25.0,0.9938799999999999,3.35,0.63,12.0,6
8.0,0.48,0.34,2.2,0.073,16.0,25.0,0.9936,3.28,0.66,12.4,6
6.3,0.57,0.28,2.1,0.048,13.0,49.0,0.99374,3.41,0.6,12.8,5
8.2,0.23,0.42,1.9,0.069,9.0,17.0,0.9937600000000001,3.21,0.54,12.3,6
9.1,0.3,0.41,2.0,0.068,10.0,24.0,0.99523,3.27,0.85,11.7,7
8.1,0.78,0.1,3.3,0.09,4.0,13.0,0.99855,3.36,0.49,9.5,5
10.8,0.47,0.43,2.1,0.171,27.0,66.0,0.9982,3.17,0.76,10.8,6
8.3,0.53,0.0,1.4,0.07,6.0,14.0,0.99593,3.25,0.64,10.0,6
5.4,0.42,0.27,2.0,0.092,23.0,55.0,0.99471,3.78,0.64,12.3,7
7.9,0.33,0.41,1.5,0.055999999999999994,6.0,35.0,0.9939600000000001,3.29,0.71,11.0,6
8.9,0.24,0.39,1.6,0.07400000000000001,3.0,10.0,0.99698,3.12,0.59,9.5,6
5.0,0.4,0.5,4.3,0.046,29.0,80.0,0.9902,3.49,0.66,13.6,6
7.0,0.69,0.07,2.5,0.091,15.0,21.0,0.99572,3.38,0.6,11.3,6
7.0,0.69,0.07,2.5,0.091,15.0,21.0,0.99572,3.38,0.6,11.3,6
7.0,0.69,0.07,2.5,0.091,15.0,21.0,0.99572,3.38,0.6,11.3,6
7.1,0.39,0.12,2.1,0.065,14.0,24.0,0.9925200000000001,3.3,0.53,13.3,6
5.6,0.66,0.0,2.5,0.066,7.0,15.0,0.99256,3.52,0.58,12.9,5
7.9,0.54,0.34,2.5,0.076,8.0,17.0,0.99235,3.2,0.72,13.1,8
6.6,0.5,0.0,1.8,0.062,21.0,28.0,0.9935200000000001,3.44,0.55,12.3,6
6.3,0.47,0.0,1.4,0.055,27.0,33.0,0.9922,3.45,0.48,12.3,6
10.7,0.4,0.37,1.9,0.081,17.0,29.0,0.99674,3.12,0.65,11.2,6
6.5,0.58,0.0,2.2,0.096,3.0,13.0,0.9955700000000001,3.62,0.62,11.5,4
8.8,0.24,0.35,1.7,0.055,13.0,27.0,0.9939399999999999,3.14,0.59,11.3,7
5.8,0.29,0.26,1.7,0.063,3.0,11.0,0.9915,3.39,0.54,13.5,6
6.3,0.76,0.0,2.9,0.07200000000000001,26.0,52.0,0.99379,3.51,0.6,11.5,6
10.0,0.43,0.33,2.7,0.095,28.0,89.0,0.9984,3.22,0.68,10.0,5
10.5,0.43,0.35,3.3,0.092,24.0,70.0,0.99798,3.21,0.69,10.5,6
9.1,0.6,0.0,1.9,0.057999999999999996,5.0,10.0,0.9977,3.18,0.63,10.4,6
5.9,0.19,0.21,1.7,0.045,57.0,135.0,0.99341,3.32,0.44,9.5,5
7.4,0.36,0.34,1.8,0.075,18.0,38.0,0.9933,3.38,0.88,13.6,7
7.2,0.48,0.07,5.5,0.08900000000000001,10.0,18.0,0.99684,3.37,0.68,11.2,7
8.5,0.28,0.35,1.7,0.061,6.0,15.0,0.99524,3.3,0.74,11.8,7
8.0,0.25,0.43,1.7,0.067,22.0,50.0,0.9946,3.38,0.6,11.9,6
10.4,0.52,0.45,2.0,0.08,6.0,13.0,0.99774,3.22,0.76,11.4,6
10.4,0.52,0.45,2.0,0.08,6.0,13.0,0.99774,3.22,0.76,11.4,6
7.5,0.41,0.15,3.7,0.10400000000000001,29.0,94.0,0.9978600000000001,3.14,0.58,9.1,5
8.2,0.51,0.24,2.0,0.079,16.0,86.0,0.99764,3.34,0.64,9.5,6
7.3,0.4,0.3,1.7,0.08,33.0,79.0,0.9969,3.41,0.65,9.5,6
8.2,0.38,0.32,2.5,0.08,24.0,71.0,0.99624,3.27,0.85,11.0,6
6.9,0.45,0.11,2.4,0.043,6.0,12.0,0.99354,3.3,0.65,11.4,6
7.0,0.22,0.3,1.8,0.065,16.0,20.0,0.99672,3.61,0.82,10.0,6
7.3,0.32,0.23,2.3,0.066,35.0,70.0,0.9958799999999999,3.43,0.62,10.1,5
8.2,0.2,0.43,2.5,0.076,31.0,51.0,0.99672,3.53,0.81,10.4,6
7.8,0.5,0.12,1.8,0.17800000000000002,6.0,21.0,0.996,3.28,0.87,9.8,6
10.0,0.41,0.45,6.2,0.071,6.0,14.0,0.99702,3.21,0.49,11.8,7
7.8,0.39,0.42,2.0,0.086,9.0,21.0,0.99526,3.39,0.66,11.6,6
10.0,0.35,0.47,2.0,0.061,6.0,11.0,0.99585,3.23,0.52,12.0,6
8.2,0.33,0.32,2.8,0.067,4.0,12.0,0.9947299999999999,3.3,0.76,12.8,7
6.1,0.58,0.23,2.5,0.044000000000000004,16.0,70.0,0.9935200000000001,3.46,0.65,12.5,6
8.3,0.6,0.25,2.2,0.11800000000000001,9.0,38.0,0.99616,3.15,0.53,9.8,5
9.6,0.42,0.35,2.1,0.083,17.0,38.0,0.9962200000000001,3.23,0.66,11.1,6
6.6,0.58,0.0,2.2,0.1,50.0,63.0,0.99544,3.59,0.68,11.4,6
8.3,0.6,0.25,2.2,0.11800000000000001,9.0,38.0,0.99616,3.15,0.53,9.8,5
8.5,0.18,0.51,1.75,0.071,45.0,88.0,0.99524,3.33,0.76,11.8,7
5.1,0.51,0.18,2.1,0.042,16.0,101.0,0.9924,3.46,0.87,12.9,7
6.7,0.41,0.43,2.8,0.076,22.0,54.0,0.99572,3.42,1.16,10.6,6
10.2,0.41,0.43,2.2,0.11,11.0,37.0,0.9972799999999999,3.16,0.67,10.8,5
10.6,0.36,0.57,2.3,0.087,6.0,20.0,0.9967600000000001,3.14,0.72,11.1,7
8.8,0.45,0.43,1.4,0.076,12.0,21.0,0.99551,3.21,0.75,10.2,6
8.5,0.32,0.42,2.3,0.075,12.0,19.0,0.99434,3.14,0.71,11.8,7
9.0,0.785,0.24,1.7,0.078,10.0,21.0,0.99692,3.29,0.67,10.0,5
9.0,0.785,0.24,1.7,0.078,10.0,21.0,0.99692,3.29,0.67,10.0,5
8.5,0.44,0.5,1.9,0.369,15.0,38.0,0.99634,3.01,1.1,9.4,5
9.9,0.54,0.26,2.0,0.111,7.0,60.0,0.9970899999999999,2.94,0.98,10.2,5
8.2,0.33,0.39,2.5,0.07400000000000001,29.0,48.0,0.9952799999999999,3.32,0.88,12.4,7
6.5,0.34,0.27,2.8,0.067,8.0,44.0,0.99384,3.21,0.56,12.0,6
7.6,0.5,0.29,2.3,0.086,5.0,14.0,0.9950200000000001,3.32,0.62,11.5,6
9.2,0.36,0.34,1.6,0.062,5.0,12.0,0.9966700000000001,3.2,0.67,10.5,6
7.1,0.59,0.0,2.2,0.078,26.0,44.0,0.9952200000000001,3.42,0.68,10.8,6
9.7,0.42,0.46,2.1,0.07400000000000001,5.0,16.0,0.99649,3.27,0.74,12.3,6
7.6,0.36,0.31,1.7,0.079,26.0,65.0,0.99716,3.46,0.62,9.5,6
7.6,0.36,0.31,1.7,0.079,26.0,65.0,0.99716,3.46,0.62,9.5,6
6.5,0.61,0.0,2.2,0.095,48.0,59.0,0.99541,3.61,0.7,11.5,6
6.5,0.88,0.03,5.6,0.079,23.0,47.0,0.99572,3.58,0.5,11.2,4
7.1,0.66,0.0,2.4,0.052000000000000005,6.0,11.0,0.99318,3.35,0.66,12.7,7
5.6,0.915,0.0,2.1,0.040999999999999995,17.0,78.0,0.99346,3.68,0.73,11.4,5
8.2,0.35,0.33,2.4,0.076,11.0,47.0,0.9959899999999999,3.27,0.81,11.0,6
8.2,0.35,0.33,2.4,0.076,11.0,47.0,0.9959899999999999,3.27,0.81,11.0,6
9.8,0.39,0.43,1.65,0.068,5.0,11.0,0.9947799999999999,3.19,0.46,11.4,5
10.2,0.4,0.4,2.5,0.068,41.0,54.0,0.99754,3.38,0.86,10.5,6
6.8,0.66,0.07,1.6,0.07,16.0,61.0,0.99572,3.29,0.6,9.3,5
6.7,0.64,0.23,2.1,0.08,11.0,119.0,0.9953799999999999,3.36,0.7,10.9,5
7.0,0.43,0.3,2.0,0.085,6.0,39.0,0.99346,3.33,0.46,11.9,6
6.6,0.8,0.03,7.8,0.079,6.0,12.0,0.9963,3.52,0.5,12.2,5
7.0,0.43,0.3,2.0,0.085,6.0,39.0,0.99346,3.33,0.46,11.9,6
6.7,0.64,0.23,2.1,0.08,11.0,119.0,0.9953799999999999,3.36,0.7,10.9,5
8.8,0.955,0.05,1.8,0.075,5.0,19.0,0.99616,3.3,0.44,9.6,4
9.1,0.4,0.57,4.6,0.08,6.0,20.0,0.9965200000000001,3.28,0.57,12.5,6
6.5,0.885,0.0,2.3,0.166,6.0,12.0,0.99551,3.56,0.51,10.8,5
7.2,0.25,0.37,2.5,0.063,11.0,41.0,0.99439,3.52,0.8,12.4,7
6.4,0.885,0.0,2.3,0.166,6.0,12.0,0.99551,3.56,0.51,10.8,5
7.0,0.745,0.12,1.8,0.114,15.0,64.0,0.9958799999999999,3.22,0.59,9.5,6
6.2,0.43,0.22,1.8,0.078,21.0,56.0,0.9963299999999999,3.52,0.6,9.5,6
7.9,0.58,0.23,2.3,0.076,23.0,94.0,0.9968600000000001,3.21,0.58,9.5,6
7.7,0.57,0.21,1.5,0.069,4.0,9.0,0.9945799999999999,3.16,0.54,9.8,6
7.7,0.26,0.26,2.0,0.052000000000000005,19.0,77.0,0.9951,3.15,0.79,10.9,6
7.9,0.58,0.23,2.3,0.076,23.0,94.0,0.9968600000000001,3.21,0.58,9.5,6
7.7,0.57,0.21,1.5,0.069,4.0,9.0,0.9945799999999999,3.16,0.54,9.8,6
7.9,0.34,0.36,1.9,0.065,5.0,10.0,0.99419,3.27,0.54,11.2,7
8.6,0.42,0.39,1.8,0.068,6.0,12.0,0.99516,3.35,0.69,11.7,8
9.9,0.74,0.19,5.8,0.111,33.0,76.0,0.9987799999999999,3.14,0.55,9.4,5
7.2,0.36,0.46,2.1,0.07400000000000001,24.0,44.0,0.99534,3.4,0.85,11.0,7
7.2,0.36,0.46,2.1,0.07400000000000001,24.0,44.0,0.99534,3.4,0.85,11.0,7
7.2,0.36,0.46,2.1,0.07400000000000001,24.0,44.0,0.99534,3.4,0.85,11.0,7
9.9,0.72,0.55,1.7,0.136,24.0,52.0,0.9975200000000001,3.35,0.94,10.0,5
7.2,0.36,0.46,2.1,0.07400000000000001,24.0,44.0,0.99534,3.4,0.85,11.0,7
6.2,0.39,0.43,2.0,0.071,14.0,24.0,0.9942799999999999,3.45,0.87,11.2,7
6.8,0.65,0.02,2.1,0.078,8.0,15.0,0.99498,3.35,0.62,10.4,6
6.6,0.44,0.15,2.1,0.076,22.0,53.0,0.9957,3.32,0.62,9.3,5
6.8,0.65,0.02,2.1,0.078,8.0,15.0,0.99498,3.35,0.62,10.4,6
9.6,0.38,0.42,1.9,0.071,5.0,13.0,0.99659,3.15,0.75,10.5,6
10.2,0.33,0.46,1.9,0.081,6.0,9.0,0.9962799999999999,3.1,0.48,10.4,6
8.8,0.27,0.46,2.1,0.095,20.0,29.0,0.9948799999999999,3.26,0.56,11.3,6
7.9,0.57,0.31,2.0,0.079,10.0,79.0,0.99677,3.29,0.69,9.5,6
8.2,0.34,0.37,1.9,0.057,43.0,74.0,0.99408,3.23,0.81,12.0,6
8.2,0.4,0.31,1.9,0.08199999999999999,8.0,24.0,0.996,3.24,0.69,10.6,6
9.0,0.39,0.4,1.3,0.044000000000000004,25.0,50.0,0.9947799999999999,3.2,0.83,10.9,6
10.9,0.32,0.52,1.8,0.132,17.0,44.0,0.99734,3.28,0.77,11.5,6
10.9,0.32,0.52,1.8,0.132,17.0,44.0,0.99734,3.28,0.77,11.5,6
8.1,0.53,0.22,2.2,0.078,33.0,89.0,0.9967799999999999,3.26,0.46,9.6,6
10.5,0.36,0.47,2.2,0.07400000000000001,9.0,23.0,0.9963799999999999,3.23,0.76,12.0,6
12.6,0.39,0.49,2.5,0.08,8.0,20.0,0.9992,3.07,0.82,10.3,6
9.2,0.46,0.23,2.6,0.091,18.0,77.0,0.9992200000000001,3.15,0.51,9.4,5
7.5,0.58,0.03,4.1,0.08,27.0,46.0,0.99592,3.02,0.47,9.2,5
9.0,0.58,0.25,2.0,0.10400000000000001,8.0,21.0,0.99769,3.27,0.72,9.6,5
5.1,0.42,0.0,1.8,0.044000000000000004,18.0,88.0,0.9915700000000001,3.68,0.73,13.6,7
7.6,0.43,0.29,2.1,0.075,19.0,66.0,0.99718,3.4,0.64,9.5,5
7.7,0.18,0.34,2.7,0.066,15.0,58.0,0.9947,3.37,0.78,11.8,6
7.8,0.815,0.01,2.6,0.07400000000000001,48.0,90.0,0.99621,3.38,0.62,10.8,5
7.6,0.43,0.29,2.1,0.075,19.0,66.0,0.99718,3.4,0.64,9.5,5
10.2,0.23,0.37,2.2,0.057,14.0,36.0,0.9961399999999999,3.23,0.49,9.3,4
7.1,0.75,0.01,2.2,0.059000000000000004,11.0,18.0,0.9924200000000001,3.39,0.4,12.8,6
6.0,0.33,0.32,12.9,0.054000000000000006,6.0,113.0,0.99572,3.3,0.56,11.5,4
7.8,0.55,0.0,1.7,0.07,7.0,17.0,0.99659,3.26,0.64,9.4,6
7.1,0.75,0.01,2.2,0.059000000000000004,11.0,18.0,0.9924200000000001,3.39,0.4,12.8,6
8.1,0.73,0.0,2.5,0.081,12.0,24.0,0.99798,3.38,0.46,9.6,4
6.5,0.67,0.0,4.3,0.057,11.0,20.0,0.9948799999999999,3.45,0.56,11.8,4
7.5,0.61,0.2,1.7,0.076,36.0,60.0,0.9949399999999999,3.1,0.4,9.3,5
9.8,0.37,0.39,2.5,0.079,28.0,65.0,0.99729,3.16,0.59,9.8,5
9.0,0.4,0.41,2.0,0.057999999999999996,15.0,40.0,0.9941399999999999,3.22,0.6,12.2,6
8.3,0.56,0.22,2.4,0.08199999999999999,10.0,86.0,0.9983,3.37,0.62,9.5,5
5.9,0.29,0.25,13.4,0.067,72.0,160.0,0.99721,3.33,0.54,10.3,6
7.4,0.55,0.19,1.8,0.08199999999999999,15.0,34.0,0.99655,3.49,0.68,10.5,5
7.4,0.74,0.07,1.7,0.086,15.0,48.0,0.9950200000000001,3.12,0.48,10.0,5
7.4,0.55,0.19,1.8,0.08199999999999999,15.0,34.0,0.99655,3.49,0.68,10.5,5
6.9,0.41,0.33,2.2,0.081,22.0,36.0,0.9949,3.41,0.75,11.1,6
7.1,0.6,0.01,2.3,0.079,24.0,37.0,0.9951399999999999,3.4,0.61,10.9,6
7.1,0.6,0.01,2.3,0.079,24.0,37.0,0.9951399999999999,3.4,0.61,10.9,6
7.5,0.58,0.14,2.2,0.077,27.0,60.0,0.9963,3.28,0.59,9.8,5
7.1,0.72,0.0,1.8,0.12300000000000001,6.0,14.0,0.9962700000000001,3.45,0.58,9.8,5
7.9,0.66,0.0,1.4,0.096,6.0,13.0,0.99569,3.43,0.58,9.5,5
7.8,0.7,0.06,1.9,0.079,20.0,35.0,0.9962799999999999,3.4,0.69,10.9,5
6.1,0.64,0.02,2.4,0.069,26.0,46.0,0.9935799999999999,3.47,0.45,11.0,5
7.5,0.59,0.22,1.8,0.08199999999999999,43.0,60.0,0.9949899999999999,3.1,0.42,9.2,5
7.0,0.58,0.28,4.8,0.085,12.0,69.0,0.9963299999999999,3.32,0.7,11.0,6
6.8,0.64,0.0,2.7,0.12300000000000001,15.0,33.0,0.9953799999999999,3.44,0.63,11.3,6
6.8,0.64,0.0,2.7,0.12300000000000001,15.0,33.0,0.9953799999999999,3.44,0.63,11.3,6
8.6,0.635,0.68,1.8,0.40299999999999997,19.0,56.0,0.9963200000000001,3.02,1.15,9.3,5
6.3,1.02,0.0,2.0,0.083,17.0,24.0,0.9943700000000001,3.59,0.55,11.2,4
9.8,0.45,0.38,2.5,0.081,34.0,66.0,0.99726,3.15,0.58,9.8,5
8.2,0.78,0.0,2.2,0.08900000000000001,13.0,26.0,0.9978,3.37,0.46,9.6,4
8.5,0.37,0.32,1.8,0.066,26.0,51.0,0.99456,3.38,0.72,11.8,6
7.2,0.57,0.05,2.3,0.081,16.0,36.0,0.99564,3.38,0.6,10.3,6
7.2,0.57,0.05,2.3,0.081,16.0,36.0,0.99564,3.38,0.6,10.3,6
10.4,0.43,0.5,2.3,0.068,13.0,19.0,0.996,3.1,0.87,11.4,6
6.9,0.41,0.31,2.0,0.079,21.0,51.0,0.9966799999999999,3.47,0.55,9.5,6
5.5,0.49,0.03,1.8,0.044000000000000004,28.0,87.0,0.9908,3.5,0.82,14.0,8
5.0,0.38,0.01,1.6,0.048,26.0,60.0,0.9908399999999999,3.7,0.75,14.0,6
7.3,0.44,0.2,1.6,0.049,24.0,64.0,0.9935,3.38,0.57,11.7,6
5.9,0.46,0.0,1.9,0.077,25.0,44.0,0.99385,3.5,0.53,11.2,5
7.5,0.58,0.2,2.0,0.073,34.0,44.0,0.9949399999999999,3.1,0.43,9.3,5
7.8,0.58,0.13,2.1,0.102,17.0,36.0,0.9944,3.24,0.53,11.2,6
8.0,0.715,0.22,2.3,0.075,13.0,81.0,0.9968799999999999,3.24,0.54,9.5,6
8.5,0.4,0.4,6.3,0.05,3.0,10.0,0.99566,3.28,0.56,12.0,4
7.0,0.69,0.0,1.9,0.114,3.0,10.0,0.99636,3.35,0.6,9.7,6
8.0,0.715,0.22,2.3,0.075,13.0,81.0,0.9968799999999999,3.24,0.54,9.5,6
9.8,0.3,0.39,1.7,0.062,3.0,9.0,0.9948,3.14,0.57,11.5,7
7.1,0.46,0.2,1.9,0.077,28.0,54.0,0.9956,3.37,0.64,10.4,6
7.1,0.46,0.2,1.9,0.077,28.0,54.0,0.9956,3.37,0.64,10.4,6
7.9,0.765,0.0,2.0,0.084,9.0,22.0,0.9961899999999999,3.33,0.68,10.9,6
8.7,0.63,0.28,2.7,0.096,17.0,69.0,0.99734,3.26,0.63,10.2,6
7.0,0.42,0.19,2.3,0.071,18.0,36.0,0.9947600000000001,3.39,0.56,10.9,5
11.3,0.37,0.5,1.8,0.09,20.0,47.0,0.99734,3.15,0.57,10.5,5
7.1,0.16,0.44,2.5,0.068,17.0,31.0,0.9932799999999999,3.35,0.54,12.4,6
8.0,0.6,0.08,2.6,0.055999999999999994,3.0,7.0,0.9928600000000001,3.22,0.37,13.0,5
7.0,0.6,0.3,4.5,0.068,20.0,110.0,0.9991399999999999,3.3,1.17,10.2,5
7.0,0.6,0.3,4.5,0.068,20.0,110.0,0.9991399999999999,3.3,1.17,10.2,5
7.6,0.74,0.0,1.9,0.1,6.0,12.0,0.99521,3.36,0.59,11.0,5
8.2,0.635,0.1,2.1,0.073,25.0,60.0,0.9963799999999999,3.29,0.75,10.9,6
5.9,0.395,0.13,2.4,0.055999999999999994,14.0,28.0,0.9936200000000001,3.62,0.67,12.4,6
7.5,0.755,0.0,1.9,0.084,6.0,12.0,0.99672,3.34,0.49,9.7,4
8.2,0.635,0.1,2.1,0.073,25.0,60.0,0.9963799999999999,3.29,0.75,10.9,6
6.6,0.63,0.0,4.3,0.09300000000000001,51.0,77.5,0.9955799999999999,3.2,0.45,9.5,5
6.6,0.63,0.0,4.3,0.09300000000000001,51.0,77.5,0.9955799999999999,3.2,0.45,9.5,5
7.2,0.53,0.14,2.1,0.064,15.0,29.0,0.99323,3.35,0.61,12.1,6
5.7,0.6,0.0,1.4,0.063,11.0,18.0,0.9919100000000001,3.45,0.56,12.2,6
7.6,1.58,0.0,2.1,0.13699999999999998,5.0,9.0,0.9947600000000001,3.5,0.4,10.9,3
5.2,0.645,0.0,2.15,0.08,15.0,28.0,0.99444,3.78,0.61,12.5,6
6.7,0.86,0.07,2.0,0.1,20.0,57.0,0.99598,3.6,0.74,11.7,6
9.1,0.37,0.32,2.1,0.064,4.0,15.0,0.9957600000000001,3.3,0.8,11.2,6
8.0,0.28,0.44,1.8,0.081,28.0,68.0,0.9950100000000001,3.36,0.66,11.2,5
7.6,0.79,0.21,2.3,0.087,21.0,68.0,0.9955,3.12,0.44,9.2,5
7.5,0.61,0.26,1.9,0.073,24.0,88.0,0.9961200000000001,3.3,0.53,9.8,5
9.7,0.69,0.32,2.5,0.08800000000000001,22.0,91.0,0.9979,3.29,0.62,10.1,5
6.8,0.68,0.09,3.9,0.068,15.0,29.0,0.99524,3.41,0.52,11.1,4
9.7,0.69,0.32,2.5,0.08800000000000001,22.0,91.0,0.9979,3.29,0.62,10.1,5
7.0,0.62,0.1,1.4,0.071,27.0,63.0,0.996,3.28,0.61,9.2,5
7.5,0.61,0.26,1.9,0.073,24.0,88.0,0.9961200000000001,3.3,0.53,9.8,5
6.5,0.51,0.15,3.0,0.064,12.0,27.0,0.9929,3.33,0.59,12.8,6
8.0,1.18,0.21,1.9,0.083,14.0,41.0,0.9953200000000001,3.34,0.47,10.5,5
7.0,0.36,0.21,2.3,0.086,20.0,65.0,0.9955799999999999,3.4,0.54,10.1,6
7.0,0.36,0.21,2.4,0.086,24.0,69.0,0.99556,3.4,0.53,10.1,6
7.5,0.63,0.27,2.0,0.083,17.0,91.0,0.99616,3.26,0.58,9.8,6
5.4,0.74,0.0,1.2,0.040999999999999995,16.0,46.0,0.9925799999999999,4.01,0.59,12.5,6
9.9,0.44,0.46,2.2,0.091,10.0,41.0,0.9963799999999999,3.18,0.69,11.9,6
7.5,0.63,0.27,2.0,0.083,17.0,91.0,0.99616,3.26,0.58,9.8,6
9.1,0.76,0.68,1.7,0.414,18.0,64.0,0.9965200000000001,2.9,1.33,9.1,6
9.7,0.66,0.34,2.6,0.094,12.0,88.0,0.9979600000000001,3.26,0.66,10.1,5
5.0,0.74,0.0,1.2,0.040999999999999995,16.0,46.0,0.9925799999999999,4.01,0.59,12.5,6
9.1,0.34,0.42,1.8,0.057999999999999996,9.0,18.0,0.99392,3.18,0.55,11.4,5
9.1,0.36,0.39,1.8,0.06,21.0,55.0,0.99495,3.18,0.82,11.0,7
6.7,0.46,0.24,1.7,0.077,18.0,34.0,0.9948,3.39,0.6,10.6,6
6.7,0.46,0.24,1.7,0.077,18.0,34.0,0.9948,3.39,0.6,10.6,6
6.7,0.46,0.24,1.7,0.077,18.0,34.0,0.9948,3.39,0.6,10.6,6
6.7,0.46,0.24,1.7,0.077,18.0,34.0,0.9948,3.39,0.6,10.6,6
6.5,0.52,0.11,1.8,0.073,13.0,38.0,0.9955,3.34,0.52,9.3,5
7.4,0.6,0.26,2.1,0.083,17.0,91.0,0.99616,3.29,0.56,9.8,6
7.4,0.6,0.26,2.1,0.083,17.0,91.0,0.99616,3.29,0.56,9.8,6
7.8,0.87,0.26,3.8,0.107,31.0,67.0,0.9966799999999999,3.26,0.46,9.2,5
8.4,0.39,0.1,1.7,0.075,6.0,25.0,0.9958100000000001,3.09,0.43,9.7,6
9.1,0.775,0.22,2.2,0.079,12.0,48.0,0.9976,3.18,0.51,9.6,5
7.2,0.835,0.0,2.0,0.166,4.0,11.0,0.99608,3.39,0.52,10.0,5
6.6,0.58,0.02,2.4,0.069,19.0,40.0,0.99387,3.38,0.66,12.6,6
6.0,0.5,0.0,1.4,0.057,15.0,26.0,0.9944799999999999,3.36,0.45,9.5,5
6.0,0.5,0.0,1.4,0.057,15.0,26.0,0.9944799999999999,3.36,0.45,9.5,5
6.0,0.5,0.0,1.4,0.057,15.0,26.0,0.9944799999999999,3.36,0.45,9.5,5
7.5,0.51,0.02,1.7,0.084,13.0,31.0,0.9953799999999999,3.36,0.54,10.5,6
7.5,0.51,0.02,1.7,0.084,13.0,31.0,0.9953799999999999,3.36,0.54,10.5,6
7.5,0.51,0.02,1.7,0.084,13.0,31.0,0.9953799999999999,3.36,0.54,10.5,6
7.6,0.54,0.02,1.7,0.085,17.0,31.0,0.9958899999999999,3.37,0.51,10.4,6
7.5,0.51,0.02,1.7,0.084,13.0,31.0,0.9953799999999999,3.36,0.54,10.5,6
11.5,0.42,0.48,2.6,0.077,8.0,20.0,0.9985200000000001,3.09,0.53,11.0,5
8.2,0.44,0.24,2.3,0.063,10.0,28.0,0.99613,3.25,0.53,10.2,6
6.1,0.59,0.01,2.1,0.055999999999999994,5.0,13.0,0.99472,3.52,0.56,11.4,5
7.2,0.655,0.03,1.8,0.078,7.0,12.0,0.99587,3.34,0.39,9.5,5
7.2,0.655,0.03,1.8,0.078,7.0,12.0,0.99587,3.34,0.39,9.5,5
6.9,0.57,0.0,2.8,0.081,21.0,41.0,0.99518,3.41,0.52,10.8,5
9.0,0.6,0.29,2.0,0.069,32.0,73.0,0.99654,3.34,0.57,10.0,5
7.2,0.62,0.01,2.3,0.065,8.0,46.0,0.9933200000000001,3.32,0.51,11.8,6
7.6,0.645,0.03,1.9,0.086,14.0,57.0,0.9969,3.37,0.46,10.3,5
7.6,0.645,0.03,1.9,0.086,14.0,57.0,0.9969,3.37,0.46,10.3,5
7.2,0.58,0.03,2.3,0.077,7.0,28.0,0.9956799999999999,3.35,0.52,10.0,5
6.1,0.32,0.25,1.8,0.086,5.0,32.0,0.99464,3.36,0.44,10.1,5
6.1,0.34,0.25,1.8,0.084,4.0,28.0,0.99464,3.36,0.44,10.1,5
7.3,0.43,0.24,2.5,0.078,27.0,67.0,0.9964799999999999,3.6,0.59,11.1,6
7.4,0.64,0.17,5.4,0.168,52.0,98.0,0.99736,3.28,0.5,9.5,5
11.6,0.475,0.4,1.4,0.091,6.0,28.0,0.9970399999999999,3.07,0.65,10.0333333333333,6
9.2,0.54,0.31,2.3,0.11199999999999999,11.0,38.0,0.9969899999999999,3.24,0.56,10.9,5
8.3,0.85,0.14,2.5,0.09300000000000001,13.0,54.0,0.99724,3.36,0.54,10.1,5
11.6,0.475,0.4,1.4,0.091,6.0,28.0,0.9970399999999999,3.07,0.65,10.0333333333333,6
8.0,0.83,0.27,2.0,0.08,11.0,63.0,0.9965200000000001,3.29,0.48,9.8,4
7.2,0.605,0.02,1.9,0.096,10.0,31.0,0.995,3.46,0.53,11.8,6
7.8,0.5,0.09,2.2,0.115,10.0,42.0,0.9971,3.18,0.62,9.5,5
7.3,0.74,0.08,1.7,0.094,10.0,45.0,0.9957600000000001,3.24,0.5,9.8,5
6.9,0.54,0.3,2.2,0.08800000000000001,9.0,105.0,0.99725,3.25,1.18,10.5,6
8.0,0.77,0.32,2.1,0.079,16.0,74.0,0.99656,3.27,0.5,9.8,6
6.6,0.61,0.0,1.6,0.069,4.0,8.0,0.9939600000000001,3.33,0.37,10.4,4
8.7,0.78,0.51,1.7,0.415,12.0,66.0,0.99623,3.0,1.17,9.2,5
7.5,0.58,0.56,3.1,0.153,5.0,14.0,0.9947600000000001,3.21,1.03,11.6,6
8.7,0.78,0.51,1.7,0.415,12.0,66.0,0.99623,3.0,1.17,9.2,5
7.7,0.75,0.27,3.8,0.11,34.0,89.0,0.99664,3.24,0.45,9.3,5
6.8,0.815,0.0,1.2,0.267,16.0,29.0,0.99471,3.32,0.51,9.8,3
7.2,0.56,0.26,2.0,0.083,13.0,100.0,0.9958600000000001,3.26,0.52,9.9,5
8.2,0.885,0.2,1.4,0.086,7.0,31.0,0.9946,3.11,0.46,10.0,5
5.2,0.49,0.26,2.3,0.09,23.0,74.0,0.9953,3.71,0.62,12.2,6
7.2,0.45,0.15,2.0,0.078,10.0,28.0,0.9960899999999999,3.29,0.51,9.9,6
7.5,0.57,0.02,2.6,0.077,11.0,35.0,0.9955700000000001,3.36,0.62,10.8,6
7.5,0.57,0.02,2.6,0.077,11.0,35.0,0.9955700000000001,3.36,0.62,10.8,6
6.8,0.83,0.09,1.8,0.07400000000000001,4.0,25.0,0.99534,3.38,0.45,9.6,5
8.0,0.6,0.22,2.1,0.08,25.0,105.0,0.99613,3.3,0.49,9.9,5
8.0,0.6,0.22,2.1,0.08,25.0,105.0,0.99613,3.3,0.49,9.9,5
7.1,0.755,0.15,1.8,0.107,20.0,84.0,0.99593,3.19,0.5,9.5,5
8.0,0.81,0.25,3.4,0.076,34.0,85.0,0.9966799999999999,3.19,0.42,9.2,5
7.4,0.64,0.07,1.8,0.1,8.0,23.0,0.9961,3.3,0.58,9.6,5
7.4,0.64,0.07,1.8,0.1,8.0,23.0,0.9961,3.3,0.58,9.6,5
6.6,0.64,0.31,6.1,0.083,7.0,49.0,0.99718,3.35,0.68,10.3,5
6.7,0.48,0.02,2.2,0.08,36.0,111.0,0.99524,3.1,0.53,9.7,5
6.0,0.49,0.0,2.3,0.068,15.0,33.0,0.99292,3.58,0.59,12.5,6
8.0,0.64,0.22,2.4,0.094,5.0,33.0,0.9961200000000001,3.37,0.58,11.0,5
7.1,0.62,0.06,1.3,0.07,5.0,12.0,0.9942,3.17,0.48,9.8,5
8.0,0.52,0.25,2.0,0.078,19.0,59.0,0.9961200000000001,3.3,0.48,10.2,5
6.4,0.57,0.14,3.9,0.07,27.0,73.0,0.99669,3.32,0.48,9.2,5
8.6,0.685,0.1,1.6,0.092,3.0,12.0,0.99745,3.31,0.65,9.55,6
8.7,0.675,0.1,1.6,0.09,4.0,11.0,0.99745,3.31,0.65,9.55,5
7.3,0.59,0.26,2.0,0.08,17.0,104.0,0.99584,3.28,0.52,9.9,5
7.0,0.6,0.12,2.2,0.083,13.0,28.0,0.9966,3.52,0.62,10.2,7
7.2,0.67,0.0,2.2,0.068,10.0,24.0,0.9956,3.42,0.72,11.1,6
7.9,0.69,0.21,2.1,0.08,33.0,141.0,0.9962,3.25,0.51,9.9,5
7.9,0.69,0.21,2.1,0.08,33.0,141.0,0.9962,3.25,0.51,9.9,5
7.6,0.3,0.42,2.0,0.052000000000000005,6.0,24.0,0.9963,3.44,0.82,11.9,6
7.2,0.33,0.33,1.7,0.061,3.0,13.0,0.996,3.23,1.1,10.0,8
8.0,0.5,0.39,2.6,0.08199999999999999,12.0,46.0,0.9985,3.43,0.62,10.7,6
7.7,0.28,0.3,2.0,0.062,18.0,34.0,0.9952,3.28,0.9,11.3,7
8.2,0.24,0.34,5.1,0.062,8.0,22.0,0.9974,3.22,0.94,10.9,6
6.0,0.51,0.0,2.1,0.064,40.0,54.0,0.995,3.54,0.93,10.7,6
8.1,0.29,0.36,2.2,0.048,35.0,53.0,0.995,3.27,1.01,12.4,7
6.0,0.51,0.0,2.1,0.064,40.0,54.0,0.995,3.54,0.93,10.7,6
6.6,0.96,0.0,1.8,0.08199999999999999,5.0,16.0,0.9936,3.5,0.44,11.9,6
6.4,0.47,0.4,2.4,0.071,8.0,19.0,0.9963,3.56,0.73,10.6,6
8.2,0.24,0.34,5.1,0.062,8.0,22.0,0.9974,3.22,0.94,10.9,6
9.9,0.57,0.25,2.0,0.10400000000000001,12.0,89.0,0.9963,3.04,0.9,10.1,5
10.0,0.32,0.59,2.2,0.077,3.0,15.0,0.9994,3.2,0.78,9.6,5
6.2,0.58,0.0,1.6,0.065,8.0,18.0,0.9966,3.56,0.84,9.4,5
10.0,0.32,0.59,2.2,0.077,3.0,15.0,0.9994,3.2,0.78,9.6,5
7.3,0.34,0.33,2.5,0.064,21.0,37.0,0.9952,3.35,0.77,12.1,7
7.8,0.53,0.01,1.6,0.077,3.0,19.0,0.995,3.16,0.46,9.8,5
7.7,0.64,0.21,2.2,0.077,32.0,133.0,0.9956,3.27,0.45,9.9,5
7.8,0.53,0.01,1.6,0.077,3.0,19.0,0.995,3.16,0.46,9.8,5
7.5,0.4,0.18,1.6,0.079,24.0,58.0,0.9965,3.34,0.58,9.4,5
7.0,0.54,0.0,2.1,0.079,39.0,55.0,0.9956,3.39,0.84,11.4,6
6.4,0.53,0.09,3.9,0.12300000000000001,14.0,31.0,0.9968,3.5,0.67,11.0,4
8.3,0.26,0.37,1.4,0.076,8.0,23.0,0.9974,3.26,0.7,9.6,6
8.3,0.26,0.37,1.4,0.076,8.0,23.0,0.9974,3.26,0.7,9.6,6
7.7,0.23,0.37,1.8,0.046,23.0,60.0,0.9971,3.41,0.71,12.1,6
7.6,0.41,0.33,2.5,0.078,6.0,23.0,0.9957,3.3,0.58,11.2,5
7.8,0.64,0.0,1.9,0.07200000000000001,27.0,55.0,0.9962,3.31,0.63,11.0,5
7.9,0.18,0.4,2.2,0.049,38.0,67.0,0.996,3.33,0.93,11.3,5
7.4,0.41,0.24,1.8,0.066,18.0,47.0,0.9956,3.37,0.62,10.4,5
7.6,0.43,0.31,2.1,0.069,13.0,74.0,0.9958,3.26,0.54,9.9,6
5.9,0.44,0.0,1.6,0.042,3.0,11.0,0.9944,3.48,0.85,11.7,6
6.1,0.4,0.16,1.8,0.069,11.0,25.0,0.9955,3.42,0.74,10.1,7
10.2,0.54,0.37,15.4,0.214,55.0,95.0,1.00369,3.18,0.77,9.0,6
10.2,0.54,0.37,15.4,0.214,55.0,95.0,1.00369,3.18,0.77,9.0,6
10.0,0.38,0.38,1.6,0.16899999999999998,27.0,90.0,0.9991399999999999,3.15,0.65,8.5,5
6.8,0.915,0.29,4.8,0.07,15.0,39.0,0.99577,3.53,0.54,11.1,5
7.0,0.59,0.0,1.7,0.052000000000000005,3.0,8.0,0.996,3.41,0.47,10.3,5
7.3,0.67,0.02,2.2,0.07200000000000001,31.0,92.0,0.99566,3.32,0.68,11.066666666666698,6
7.2,0.37,0.32,2.0,0.062,15.0,28.0,0.9947,3.23,0.73,11.3,7
7.4,0.785,0.19,5.2,0.094,19.0,98.0,0.99713,3.16,0.52,9.56666666666667,6
6.9,0.63,0.02,1.9,0.078,18.0,30.0,0.9971200000000001,3.4,0.75,9.8,5
6.9,0.58,0.2,1.75,0.057999999999999996,8.0,22.0,0.9932200000000001,3.38,0.49,11.7,5
7.3,0.67,0.02,2.2,0.07200000000000001,31.0,92.0,0.99566,3.32,0.68,11.1,6
7.4,0.785,0.19,5.2,0.094,19.0,98.0,0.99713,3.16,0.52,9.6,6
6.9,0.63,0.02,1.9,0.078,18.0,30.0,0.9971200000000001,3.4,0.75,9.8,5
6.8,0.67,0.0,1.9,0.08,22.0,39.0,0.9970100000000001,3.4,0.74,9.7,5
6.9,0.58,0.01,1.9,0.08,40.0,54.0,0.9968299999999999,3.4,0.73,9.7,5
7.2,0.38,0.31,2.0,0.055999999999999994,15.0,29.0,0.99472,3.23,0.76,11.3,8
7.2,0.37,0.32,2.0,0.062,15.0,28.0,0.9947,3.23,0.73,11.3,7
7.8,0.32,0.44,2.7,0.10400000000000001,8.0,17.0,0.9973200000000001,3.33,0.78,11.0,7
6.6,0.58,0.02,2.0,0.062,37.0,53.0,0.99374,3.35,0.76,11.6,7
7.6,0.49,0.33,1.9,0.07400000000000001,27.0,85.0,0.9970600000000001,3.41,0.58,9.0,5
11.7,0.45,0.63,2.2,0.073,7.0,23.0,0.99974,3.21,0.69,10.9,6
6.5,0.9,0.0,1.6,0.052000000000000005,9.0,17.0,0.99467,3.5,0.63,10.9,6
6.0,0.54,0.06,1.8,0.05,38.0,89.0,0.99236,3.3,0.5,10.55,6
7.6,0.49,0.33,1.9,0.07400000000000001,27.0,85.0,0.9970600000000001,3.41,0.58,9.0,5
8.4,0.29,0.4,1.7,0.067,8.0,20.0,0.99603,3.39,0.6,10.5,5
7.9,0.2,0.35,1.7,0.054000000000000006,7.0,15.0,0.9945799999999999,3.32,0.8,11.9,7
6.4,0.42,0.09,2.3,0.054000000000000006,34.0,64.0,0.99724,3.41,0.68,10.4,6
6.2,0.785,0.0,2.1,0.06,6.0,13.0,0.99664,3.59,0.61,10.0,4
6.8,0.64,0.03,2.3,0.075,14.0,31.0,0.99545,3.36,0.58,10.4,6
6.9,0.63,0.01,2.4,0.076,14.0,39.0,0.9952200000000001,3.34,0.53,10.8,6
6.8,0.59,0.1,1.7,0.063,34.0,53.0,0.9958,3.41,0.67,9.7,5
6.8,0.59,0.1,1.7,0.063,34.0,53.0,0.9958,3.41,0.67,9.7,5
7.3,0.48,0.32,2.1,0.062,31.0,54.0,0.9972799999999999,3.3,0.65,10.0,7
6.7,1.04,0.08,2.3,0.067,19.0,32.0,0.9964799999999999,3.52,0.57,11.0,4
7.3,0.48,0.32,2.1,0.062,31.0,54.0,0.9972799999999999,3.3,0.65,10.0,7
7.3,0.98,0.05,2.1,0.061,20.0,49.0,0.99705,3.31,0.55,9.7,3
10.0,0.69,0.11,1.4,0.084,8.0,24.0,0.9957799999999999,2.88,0.47,9.7,5
6.7,0.7,0.08,3.75,0.067,8.0,16.0,0.99334,3.43,0.52,12.6,5
7.6,0.35,0.6,2.6,0.073,23.0,44.0,0.99656,3.38,0.79,11.1,6
6.1,0.6,0.08,1.8,0.071,14.0,45.0,0.99336,3.38,0.54,11.0,5
9.9,0.5,0.5,13.8,0.205,48.0,82.0,1.00242,3.16,0.75,8.8,5
5.3,0.47,0.11,2.2,0.048,16.0,89.0,0.99182,3.54,0.88,13.566666666666698,7
9.9,0.5,0.5,13.8,0.205,48.0,82.0,1.00242,3.16,0.75,8.8,5
5.3,0.47,0.11,2.2,0.048,16.0,89.0,0.99182,3.54,0.88,13.6,7
7.1,0.875,0.05,5.7,0.08199999999999999,3.0,14.0,0.99808,3.4,0.52,10.2,3
8.2,0.28,0.6,3.0,0.10400000000000001,10.0,22.0,0.99828,3.39,0.68,10.6,5
5.6,0.62,0.03,1.5,0.08,6.0,13.0,0.99498,3.66,0.62,10.1,4
8.2,0.28,0.6,3.0,0.10400000000000001,10.0,22.0,0.99828,3.39,0.68,10.6,5
7.2,0.58,0.54,2.1,0.114,3.0,9.0,0.9971899999999999,3.33,0.57,10.3,4
8.1,0.33,0.44,1.5,0.042,6.0,12.0,0.9954200000000001,3.35,0.61,10.7,5
6.8,0.91,0.06,2.0,0.06,4.0,11.0,0.99592,3.53,0.64,10.9,4
7.0,0.655,0.16,2.1,0.07400000000000001,8.0,25.0,0.9960600000000001,3.37,0.55,9.7,5
6.8,0.68,0.21,2.1,0.07,9.0,23.0,0.99546,3.38,0.6,10.3,5
6.0,0.64,0.05,1.9,0.066,9.0,17.0,0.9949600000000001,3.52,0.78,10.6,5
5.6,0.54,0.04,1.7,0.049,5.0,13.0,0.9942,3.72,0.58,11.4,5
6.2,0.57,0.1,2.1,0.048,4.0,11.0,0.9944799999999999,3.44,0.76,10.8,6
7.1,0.22,0.49,1.8,0.039,8.0,18.0,0.99344,3.39,0.56,12.4,6
5.6,0.54,0.04,1.7,0.049,5.0,13.0,0.9942,3.72,0.58,11.4,5
6.2,0.65,0.06,1.6,0.05,6.0,18.0,0.9934799999999999,3.57,0.54,11.95,5
7.7,0.54,0.26,1.9,0.08900000000000001,23.0,147.0,0.99636,3.26,0.59,9.7,5
6.4,0.31,0.09,1.4,0.066,15.0,28.0,0.99459,3.42,0.7,10.0,7
7.0,0.43,0.02,1.9,0.08,15.0,28.0,0.99492,3.35,0.81,10.6,6
7.7,0.54,0.26,1.9,0.08900000000000001,23.0,147.0,0.99636,3.26,0.59,9.7,5
6.9,0.74,0.03,2.3,0.054000000000000006,7.0,16.0,0.99508,3.45,0.63,11.5,6
6.6,0.895,0.04,2.3,0.068,7.0,13.0,0.99582,3.53,0.58,10.8,6
6.9,0.74,0.03,2.3,0.054000000000000006,7.0,16.0,0.99508,3.45,0.63,11.5,6
7.5,0.725,0.04,1.5,0.076,8.0,15.0,0.99508,3.26,0.53,9.6,5
7.8,0.82,0.29,4.3,0.083,21.0,64.0,0.9964200000000001,3.16,0.53,9.4,5
7.3,0.585,0.18,2.4,0.078,15.0,60.0,0.9963799999999999,3.31,0.54,9.8,5
6.2,0.44,0.39,2.5,0.077,6.0,14.0,0.99555,3.51,0.69,11.0,6
7.5,0.38,0.57,2.3,0.106,5.0,12.0,0.99605,3.36,0.55,11.4,6
6.7,0.76,0.02,1.8,0.078,6.0,12.0,0.996,3.55,0.63,9.95,3
6.8,0.81,0.05,2.0,0.07,6.0,14.0,0.9956200000000001,3.51,0.66,10.8,6
7.5,0.38,0.57,2.3,0.106,5.0,12.0,0.99605,3.36,0.55,11.4,6
7.1,0.27,0.6,2.1,0.07400000000000001,17.0,25.0,0.9981399999999999,3.38,0.72,10.6,6
7.9,0.18,0.4,1.8,0.062,7.0,20.0,0.9941,3.28,0.7,11.1,5
6.4,0.36,0.21,2.2,0.047,26.0,48.0,0.99661,3.47,0.77,9.7,6
7.1,0.69,0.04,2.1,0.068,19.0,27.0,0.9971200000000001,3.44,0.67,9.8,5
6.4,0.79,0.04,2.2,0.061,11.0,17.0,0.9958799999999999,3.53,0.65,10.4,6
6.4,0.56,0.15,1.8,0.078,17.0,65.0,0.9929399999999999,3.33,0.6,10.5,6
6.9,0.84,0.21,4.1,0.07400000000000001,16.0,65.0,0.9984200000000001,3.53,0.72,9.23333333333333,6
6.9,0.84,0.21,4.1,0.07400000000000001,16.0,65.0,0.9984200000000001,3.53,0.72,9.25,6
6.1,0.32,0.25,2.3,0.071,23.0,58.0,0.9963299999999999,3.42,0.97,10.6,5
6.5,0.53,0.06,2.0,0.063,29.0,44.0,0.9948899999999999,3.38,0.83,10.3,6
7.4,0.47,0.46,2.2,0.114,7.0,20.0,0.9964700000000001,3.32,0.63,10.5,5
6.6,0.7,0.08,2.6,0.106,14.0,27.0,0.99665,3.44,0.58,10.2,5
6.5,0.53,0.06,2.0,0.063,29.0,44.0,0.9948899999999999,3.38,0.83,10.3,6
6.9,0.48,0.2,1.9,0.08199999999999999,9.0,23.0,0.99585,3.39,0.43,9.05,4
6.1,0.32,0.25,2.3,0.071,23.0,58.0,0.9963299999999999,3.42,0.97,10.6,5
6.8,0.48,0.25,2.0,0.076,29.0,61.0,0.9953,3.34,0.6,10.4,5
6.0,0.42,0.19,2.0,0.075,22.0,47.0,0.9952200000000001,3.39,0.78,10.0,6
6.7,0.48,0.08,2.1,0.064,18.0,34.0,0.9955200000000001,3.33,0.64,9.7,5
6.8,0.47,0.08,2.2,0.064,18.0,38.0,0.9955299999999999,3.3,0.65,9.6,6
7.1,0.53,0.07,1.7,0.071,15.0,24.0,0.9951,3.29,0.66,10.8,6
7.9,0.29,0.49,2.2,0.096,21.0,59.0,0.9971399999999999,3.31,0.67,10.1,6
7.1,0.69,0.08,2.1,0.063,42.0,52.0,0.99608,3.42,0.6,10.2,6
6.6,0.44,0.09,2.2,0.063,9.0,18.0,0.99444,3.42,0.69,11.3,6
6.1,0.705,0.1,2.8,0.081,13.0,28.0,0.99631,3.6,0.66,10.2,5
7.2,0.53,0.13,2.0,0.057999999999999996,18.0,22.0,0.9957299999999999,3.21,0.68,9.9,6
8.0,0.39,0.3,1.9,0.07400000000000001,32.0,84.0,0.9971700000000001,3.39,0.61,9.0,5
6.6,0.56,0.14,2.4,0.064,13.0,29.0,0.99397,3.42,0.62,11.7,7
7.0,0.55,0.13,2.2,0.075,15.0,35.0,0.9959,3.36,0.59,9.7,6
6.1,0.53,0.08,1.9,0.077,24.0,45.0,0.9952799999999999,3.6,0.68,10.3,6
5.4,0.58,0.08,1.9,0.059000000000000004,20.0,31.0,0.99484,3.5,0.64,10.2,6
6.2,0.64,0.09,2.5,0.081,15.0,26.0,0.9953799999999999,3.57,0.63,12.0,5
7.2,0.39,0.32,1.8,0.065,34.0,60.0,0.9971399999999999,3.46,0.78,9.9,5
6.2,0.52,0.08,4.4,0.071,11.0,32.0,0.99646,3.56,0.63,11.6,6
7.4,0.25,0.29,2.2,0.054000000000000006,19.0,49.0,0.99666,3.4,0.76,10.9,7
6.7,0.855,0.02,1.9,0.064,29.0,38.0,0.99472,3.3,0.56,10.75,6
11.1,0.44,0.42,2.2,0.064,14.0,19.0,0.9975799999999999,3.25,0.57,10.4,6
8.4,0.37,0.43,2.3,0.063,12.0,19.0,0.9955,3.17,0.81,11.2,7
6.5,0.63,0.33,1.8,0.059000000000000004,16.0,28.0,0.99531,3.36,0.64,10.1,6
7.0,0.57,0.02,2.0,0.07200000000000001,17.0,26.0,0.99575,3.36,0.61,10.2,5
6.3,0.6,0.1,1.6,0.048,12.0,26.0,0.99306,3.55,0.51,12.1,5
11.2,0.4,0.5,2.0,0.099,19.0,50.0,0.9978299999999999,3.1,0.58,10.4,5
7.4,0.36,0.3,1.8,0.07400000000000001,17.0,24.0,0.99419,3.24,0.7,11.4,8
7.1,0.68,0.0,2.3,0.087,17.0,26.0,0.9978299999999999,3.45,0.53,9.5,5
7.1,0.67,0.0,2.3,0.083,18.0,27.0,0.9976799999999999,3.44,0.54,9.4,5
6.3,0.68,0.01,3.7,0.10300000000000001,32.0,54.0,0.9958600000000001,3.51,0.66,11.3,6
7.3,0.735,0.0,2.2,0.08,18.0,28.0,0.99765,3.41,0.6,9.4,5
6.6,0.855,0.02,2.4,0.062,15.0,23.0,0.9962700000000001,3.54,0.6,11.0,6
7.0,0.56,0.17,1.7,0.065,15.0,24.0,0.9951399999999999,3.44,0.68,10.55,7
6.6,0.88,0.04,2.2,0.066,12.0,20.0,0.99636,3.53,0.56,9.9,5
6.6,0.855,0.02,2.4,0.062,15.0,23.0,0.9962700000000001,3.54,0.6,11.0,6
6.9,0.63,0.33,6.7,0.235,66.0,115.0,0.99787,3.22,0.56,9.5,5
7.8,0.6,0.26,2.0,0.08,31.0,131.0,0.9962200000000001,3.21,0.52,9.9,5
7.8,0.6,0.26,2.0,0.08,31.0,131.0,0.9962200000000001,3.21,0.52,9.9,5
7.8,0.6,0.26,2.0,0.08,31.0,131.0,0.9962200000000001,3.21,0.52,9.9,5
7.2,0.695,0.13,2.0,0.076,12.0,20.0,0.99546,3.29,0.54,10.1,5
7.2,0.695,0.13,2.0,0.076,12.0,20.0,0.99546,3.29,0.54,10.1,5
7.2,0.695,0.13,2.0,0.076,12.0,20.0,0.99546,3.29,0.54,10.1,5
6.7,0.67,0.02,1.9,0.061,26.0,42.0,0.9948899999999999,3.39,0.82,10.9,6
6.7,0.16,0.64,2.1,0.059000000000000004,24.0,52.0,0.9949399999999999,3.34,0.71,11.2,6
7.2,0.695,0.13,2.0,0.076,12.0,20.0,0.99546,3.29,0.54,10.1,5
7.0,0.56,0.13,1.6,0.077,25.0,42.0,0.99629,3.34,0.59,9.2,5
6.2,0.51,0.14,1.9,0.055999999999999994,15.0,34.0,0.9939600000000001,3.48,0.57,11.5,6
6.4,0.36,0.53,2.2,0.23,19.0,35.0,0.9934,3.37,0.93,12.4,6
6.4,0.38,0.14,2.2,0.038,15.0,25.0,0.9951399999999999,3.44,0.65,11.1,6
7.3,0.69,0.32,2.2,0.069,35.0,104.0,0.9963200000000001,3.33,0.51,9.5,5
6.0,0.58,0.2,2.4,0.075,15.0,50.0,0.99467,3.58,0.67,12.5,6
5.6,0.31,0.78,13.9,0.07400000000000001,23.0,92.0,0.99677,3.39,0.48,10.5,6
7.5,0.52,0.4,2.2,0.06,12.0,20.0,0.99474,3.26,0.64,11.8,6
8.0,0.3,0.63,1.6,0.081,16.0,29.0,0.9958799999999999,3.3,0.78,10.8,6
6.2,0.7,0.15,5.1,0.076,13.0,27.0,0.9962200000000001,3.54,0.6,11.9,6
6.8,0.67,0.15,1.8,0.11800000000000001,13.0,20.0,0.9954,3.42,0.67,11.3,6
6.2,0.56,0.09,1.7,0.053,24.0,32.0,0.9940200000000001,3.54,0.6,11.3,5
7.4,0.35,0.33,2.4,0.068,9.0,26.0,0.9947,3.36,0.6,11.9,6
6.2,0.56,0.09,1.7,0.053,24.0,32.0,0.9940200000000001,3.54,0.6,11.3,5
6.1,0.715,0.1,2.6,0.053,13.0,27.0,0.9936200000000001,3.57,0.5,11.9,5
6.2,0.46,0.29,2.1,0.07400000000000001,32.0,98.0,0.9957799999999999,3.33,0.62,9.8,5
6.7,0.32,0.44,2.4,0.061,24.0,34.0,0.99484,3.29,0.8,11.6,7
7.2,0.39,0.44,2.6,0.066,22.0,48.0,0.9949399999999999,3.3,0.84,11.5,6
7.5,0.31,0.41,2.4,0.065,34.0,60.0,0.99492,3.34,0.85,11.4,6
5.8,0.61,0.11,1.8,0.066,18.0,28.0,0.9948299999999999,3.55,0.66,10.9,6
7.2,0.66,0.33,2.5,0.068,34.0,102.0,0.9941399999999999,3.27,0.78,12.8,6
6.6,0.725,0.2,7.8,0.073,29.0,79.0,0.9977,3.29,0.54,9.2,5
6.3,0.55,0.15,1.8,0.077,26.0,35.0,0.9931399999999999,3.32,0.82,11.6,6
5.4,0.74,0.09,1.7,0.08900000000000001,16.0,26.0,0.9940200000000001,3.67,0.56,11.6,6
6.3,0.51,0.13,2.3,0.076,29.0,40.0,0.99574,3.42,0.75,11.0,6
6.8,0.62,0.08,1.9,0.068,28.0,38.0,0.99651,3.42,0.82,9.5,6
6.2,0.6,0.08,2.0,0.09,32.0,44.0,0.9949,3.45,0.58,10.5,5
5.9,0.55,0.1,2.2,0.062,39.0,51.0,0.9951200000000001,3.52,0.76,11.2,6
6.3,0.51,0.13,2.3,0.076,29.0,40.0,0.99574,3.42,0.75,11.0,6
5.9,0.645,0.12,2.0,0.075,32.0,44.0,0.9954700000000001,3.57,0.71,10.2,5
6.0,0.31,0.47,3.6,0.067,18.0,42.0,0.99549,3.39,0.66,11.0,6
