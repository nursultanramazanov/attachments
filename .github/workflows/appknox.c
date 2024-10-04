# This workflow uses actions that are not certified by GitHub. They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support documentation.
#
# Appknox: Leader in Mobile Application Security Testing Solutions <https://www.appknox.com/>
#
# To use this workflow, you must be an existing Appknox customer with GitHub Advanced Security (GHAS) enabled for your
# repository.
#
# If you *are not* an existing customer, click here to contact us for licensing and pricing details:
# <https://www.appknox.com/free-trial>.
#
# Instructions:
#
# 1. In your repository settings, navigate to 'Secrets' and click on 'New repository secret.' Name the
#    secret APPKNOX_ACCESS_TOKEN and paste your appknox user token into the value field. If you don't have a appknox token
#    or need to generate a new one for GitHub, visit the Appknox Platform, go to Account Settings->Developer Settings
#    and create a token labeled GitHub
#
# 2. Refer to the detailed workflow below, make any required adjustments, and then save it to your repository. After the
#    action executes, check the 'Security' tab for results

name: Appknox

on: #!/usr/bin/env php
<?php

use Symfony\Bundle\FrameworkBundle\Console\Application;
use Symfony\Component\Console\Input\ArgvInput;
use Symfony\Component\Debug\Debug;

// if you don't want to setup permissions the proper way, just uncomment the following PHP line
// read http://symfony.com/doc/current/book/installation.html#configuration-and-setup for more information
//umask(0000);

set_time_limit(0);

/**
 * @var Composer\Autoload\ClassLoader $loader
 */
$loader = require __DIR__.'/../app/autoload.php';

$input = new ArgvInput();
$env = $input->getParameterOption(['--env', '-e'], getenv('SYMFONY_ENV') ?: 'dev');
$debug = getenv('SYMFONY_DEBUG') !== '0' && !$input->hasParameterOption(['--no-debug', '']) && $env !== 'prod';

if ($debug) {
    Debug::enable();
}

$kernel = new AppKernel($env, $debug);
$application = new Application($kernel);
$application->run($input);
  push: #!/usr/bin/env php
<?php

require_once dirname(__FILE__).'/../var/SymfonyRequirements.php';

$lineSize = 70;
$symfonyRequirements = new SymfonyRequirements();
$iniPath = $symfonyRequirements->getPhpIniConfigPath();

echo_title('Symfony Requirements Checker');

echo '> PHP is using the following php.ini file:'.PHP_EOL;
if ($iniPath) {
    echo_style('green', '  '.$iniPath);
} else {
    echo_style('warning', '  WARNING: No configuration file (php.ini) used by PHP!');
}

echo PHP_EOL.PHP_EOL;

echo '> Checking Symfony requirements:'.PHP_EOL.'  ';

$messages = array();
foreach ($symfonyRequirements->getRequirements() as $req) {
    /** @var $req Requirement */
    if ($helpText = get_error_message($req, $lineSize)) {
        echo_style('red', 'E');
        $messages['error'][] = $helpText;
    } else {
        echo_style('green', '.');
    }
}

$checkPassed = empty($messages['error']);

foreach ($symfonyRequirements->getRecommendations() as $req) {
    if ($helpText = get_error_message($req, $lineSize)) {
        echo_style('yellow', 'W');
        $messages['warning'][] = $helpText;
    } else {
        echo_style('green', '.');
    }
}

if ($checkPassed) {
    echo_block('success', 'OK', 'Your system is ready to run Symfony projects');
} else {
    echo_block('error', 'ERROR', 'Your system is not ready to run Symfony projects');

    echo_title('Fix the following mandatory requirements', 'red');

    foreach ($messages['error'] as $helpText) {
        echo ' * '.$helpText.PHP_EOL;
    }
}

if (!empty($messages['warning'])) {
    echo_title('Optional recommendations to improve your setup', 'yellow');

    foreach ($messages['warning'] as $helpText) {
        echo ' * '.$helpText.PHP_EOL;
    }
}

echo PHP_EOL;
echo_style('title', 'Note');
echo '  The command console could use a different php.ini file'.PHP_EOL;
echo_style('title', '~~~~');
echo '  than the one used with your web server. To be on the'.PHP_EOL;
echo '      safe side, please check the requirements from your web'.PHP_EOL;
echo '      server using the ';
echo_style('yellow', 'web/config.php');
echo ' script.'.PHP_EOL;
echo PHP_EOL;

exit($checkPassed ? 0 : 1);

function get_error_message(Requirement $requirement, $lineSize)
{
    if ($requirement->isFulfilled()) {
        return;
    }

    $errorMessage = wordwrap($requirement->getTestMessage(), $lineSize - 3, PHP_EOL.'   ').PHP_EOL;
    $errorMessage .= '   > '.wordwrap($requirement->getHelpText(), $lineSize - 5, PHP_EOL.'   > ').PHP_EOL;

    return $errorMessage;
}

function echo_title($title, $style = null)
{
    $style = $style ?: 'title';

    echo PHP_EOL;
    echo_style($style, $title.PHP_EOL);
    echo_style($style, str_repeat('~', strlen($title)).PHP_EOL);
    echo PHP_EOL;
}

function echo_style($style, $message)
{
    // ANSI color codes
    $styles = array(
        'reset' => "\033[0m",
        'red' => "\033[31m",
        'green' => "\033[32m",
        'yellow' => "\033[33m",
        'error' => "\033[37;41m",
        'success' => "\033[37;42m",
        'title' => "\033[34m",
    );
    $supports = has_color_support();

    echo($supports ? $styles[$style] : '').$message.($supports ? $styles['reset'] : '');
}

function echo_block($style, $title, $message)
{
    $message = ' '.trim($message).' ';
    $width = strlen($message);

    echo PHP_EOL.PHP_EOL;

    echo_style($style, str_repeat(' ', $width).PHP_EOL);
    echo_style($style, str_pad(' ['.$title.']', $width, ' ', STR_PAD_RIGHT).PHP_EOL);
    echo_style($style, str_pad($message, $width, ' ', STR_PAD_RIGHT).PHP_EOL);
    echo_style($style, str_repeat(' ', $width).PHP_EOL);
}

function has_color_support()
{
    static $support;

    if (null === $support) {
        if (DIRECTORY_SEPARATOR == '\\') {
            $support = false !== getenv('ANSICON') || 'ON' === getenv('ConEmuANSI');
        } else {
            $support = function_exists('posix_isatty') && @posix_isatty(STDOUT);
        }
    }

    return $support;
}
    branches: [ "main" ]
  pull_request: bruzek02 <- function(x) {
### Produces sex estimates with the original Bruzek (2002) method from the observed values on the five main characters (PrSu, GrSN, CArc, IsPu, InfP)
### x: character vector (with possible values "F", "0", "M" or NA), is supposed to be of length 5.
### output -> returns a sex estimate ("F", "I" or "M") depending on the input vector, based on a simple majority rule.

    x <- x[!is.na(x)] # remove missing values
    nb_f <- sum(x == "F") # counts of "F" in the input vector
    nb_m <- sum(x == "M") # counts of "M" in the input vector

    ## Return a sex estimate based on a majority rule:
    if (nb_f > nb_m) {
        return("F")  
    } else if (nb_f < nb_m) {
        return("M")
    } else { # nb_f == nb_m
        return("I")
    }
}
    branches: [ "main" ]
jobs: sexingFromFile <- function(data, ref, updateProgressBar = NULL,
                           conf_level = 0.95, strategy = c("BIC", "AIC", "None"),
                           trace = 1){
    .Deprecated("dataframe_sexing")
    dataframe_sexing(data, ref, updateProgressBar,
                     conf_level, strategy, trace)
}

dataframe_sexing <- function(data, ref, updateProgressBar = NULL,
                             conf_level = 0.95, strategy = c("BIC", "AIC", "None"),
                             trace = 1) {
### data: dataframe (individuals to estimate) with at least the eleven trichotomic traits
### ref: dataframe (learning dataset)
### updateProgressBar: function for progress bar in shiny app
### conf_level: 0.95 by default, confidence level needed to produce a sex estimate
### strategy: passed to indivSexing
### trace: passed to indivSexing
### (NB : les colonnes doivent contenir les labels adéquats, mais il peut éventuellement y avoir d'autres colonnes illustratives [e.g. age, ...] dans le jeu de données)

#################################################
### 1. Add the main characters PrSu, GrSN, InfP #
#################################################
    data <- data[ , c("PrSu1", "PrSu2", "PrSu3", "GrSN1", "GrSN2", "GrSN3",
                    "CArc", "InfP1", "InfP2", "InfP3", "IsPu")]
    metadat <- add_metavars(data)

########################################################
### 2. Prepare and initialize the dataframe of results #
########################################################
    df_res <- matrix(NA, ncol = 21, nrow = nrow(data))
    df_res <- as.data.frame(df_res, stringsAsFactors = FALSE)
    colnames(df_res) <- c("PrSu1", "PrSu2", "PrSu3", "PrSu",
                          "GrSN1", "GrSN2", "GrSN3", "GrSN",
                          "CArc", "InfP1", "InfP2", "InfP3", "InfP", "IsPu",
                          "Sex estimate (Bruzek 2002)",
                          "Statistical sex estimate (2019)",
                          "Prob(F)", "Prob(M)",
                          "Selected predictors in LR model",
                          "10-fold CV accuracy (%)", "Indet. rate in CV (%)")
    rownames(df_res) <- rownames(data)

########################################
### 3. Sex estimation with Bruzek 2002 #
########################################
    b02 <- apply(metadat[ , c("PrSu", "GrSN", "InfP", "CArc", "IsPu")],
                 MARGIN = 1, FUN = bruzek02)
    df_res[ , "Sex estimate (Bruzek 2002)"] <- factor(b02)

#################################
### 4. Sex estimation with GLMs #
#################################
    index_models <- 1
    dtf_models <- data.frame(InitialVars = character(0),
                             tenCV_perfRate = numeric(0),
                             tenCV_indetRate = numeric(0),
                             stringsAsFactors = FALSE)
    list_models <- list()

    for (i in 1:nrow(data)) { # for each individual,
        ## retrieve the observed traits on this individual:
        obs_traits <- paste(colnames(data)[which(!is.na(data[i, ]))],
                            collapse = ", ")
        if ((! obs_traits %in% dtf_models$InitialVars) & (sum(!is.na(data[i, ])) > 0)) {
            ## if those variables do not correspond to a GLM that has already been used:
            res <- indiv_sexing(ref = ref, new_ind = data[i, ],
                                strategy = strategy, trace = trace,
                                conf_level = conf_level) # then we compute this GLM,
            df_res[i, "Statistical sex estimate (2019)"] <- res$PredictedSex # and we store the results in the dataframe.
            df_res[i, "Prob(F)"] <- round(1 - res$PostProb, 3)
            df_res[i, "Prob(M)"] <- round(res$PostProb, 3)
            df_res[i, "Selected predictors in LR model"] <- res$VariablesUsed
            df_res[i, "10-fold CV accuracy (%)"] <- round(100 * res$cvRate, 2)
            df_res[i, "Indet. rate in CV (%)"] <- round(100 * res$cvIndet, 2)
            list_models[[index_models]] <- res$BestModel
            dtf_models[index_models, "InitialVars"] <- obs_traits
            dtf_models[index_models, "tenCV_perfRate"] <- round(100 * res$cvRate, 2)
            dtf_models[index_models, "tenCV_indetRate"] <- round(100 * res$cvIndet, 2)
            index_models <- index_models + 1 # here we increment the index of GLMs that have been used.
        } else if (sum(!is.na(data[i,])) > 0) { # # if those variables do correspond to a GLM that has already been used,
            where.mod <- which(dtf_models$InitialVars == obs_traits) # retrieve this model,
            mod_current <- list_models[[where.mod]]
            ## and use this model to return a sex estimate:
            postprob <- predict(mod_current, newdata = as.data.frame(data[i, ]), type = "response")
            df_res[i, "Statistical sex estimate (2019)"] <- finalSE(postprob, conf_level)
            df_res[i, "Prob(F)"] <- round(1 - postprob, 3)
            df_res[i, "Prob(M)"] <- round(postprob, 3)
            df_res[i, "Selected predictors in LR model"] <- paste(attr(mod_current$terms, "term.labels"), collapse = ", ")
            df_res[i, "10-fold CV accuracy (%)"] <- dtf_models[where.mod, "tenCV_perfRate"]
            df_res[i, "Indet. rate in CV (%)"] <- dtf_models[where.mod, "tenCV_indetRate"]
        } 
        ## Update progress bar:
        if (is.function(updateProgressBar)) { # i.e., if not-NULL,
            text <- paste("Currently processing individual ", rownames(data)[i], "...", sep = "")
            updateProgressBar(detail = text, total = nrow(data))
        }
    }

##########################################
### 5. Return the results in a dataframe #
##########################################
    df_res[ , c("PrSu1", "PrSu2", "PrSu3", "GrSN1", "GrSN2", "GrSN3", "CArc", "InfP1", "InfP2", "InfP3", "IsPu")] <- data
    df_res[ , c("PrSu", "GrSN", "InfP", "CArc", "IsPu")] <- metadat[ , c("PrSu", "GrSN", "InfP", "CArc", "IsPu")]
    return(df_res)
}
  appknox: indivSexing <- function(ref, new_ind, strategy = c("BIC", "AIC", "None"),
                        trace = 1, conf_level = 0.95) {
    .Deprecated("indiv_sexing")
    indiv_sexing(ref, new_ind, strategy, trace, conf_level)
}

indiv_sexing <- function(ref, new_ind, strategy = c("BIC", "AIC", "None"),
                         trace = 1, conf_level = 0.95) {
### Returns a sex estimate of an individual "new_ind" using a GLM learned on the dataframe "ref"
### ref: dataframe, reference dataset with the eleven trichotomic traits used in Bruzek's method
### strategy: strategy for variable selection in LR model
### new_ind: vector or 1-row dataframe
### trace: passed to MASS::stepAIC
### conf_level: required posterior probability threshold to produce a sex estimate

#######################################
### 1. Prepare constants and arguments:
    ## to avoid a warning if the user do not specify anything:
    strategy <- match.arg(strategy)
    ## mandatory for subsequent use by "predict" function:
    new_ind <- as.data.frame(new_ind)

######################
### 2. Sex estimation:
    if (ncol(new_ind) > 0) {
        ## if we have a valid input,
        ## Mark columns with non-missing values (i.e., observed traits) for the new individual:
        column_ok <- colnames(new_ind)[which(! is.na(new_ind[1, ]))]
        new_ind <- as.data.frame(new_ind[ , !is.na(new_ind[1, ])])
        colnames(new_ind) <- column_ok

        ## 2.1. In "ref", keep only the traits observed in "new_ind":
        ref <- na.omit(ref[ , c("Sex", colnames(new_ind))])

        ## 2.2. Build a complete GLM on the learning dataset:
        mod.complete <- glm(Sex ~ ., data = ref, family = binomial)

        ## 2.3. Variable selection (optional):
        if (strategy == "None") {
            mod.best <- mod.complete # no variable selection
        } else if (strategy == "AIC") {
            mod.best <- MASS::stepAIC(mod.complete, k = 2, trace = trace)
        } else { # strategy = "BIC"
            mod.best <- MASS::stepAIC(mod.complete, k = log(nrow(ref)),
                                      trace = trace)
        }
        ## Make list of covariates, in one single string, separated by commas:
        predicteurs <- paste(attr(mod.best$terms, "term.labels"),
                             collapse = ", ")

        ## 2.4. Posterior probabilities predicted for "new_ind":
        postprob <- predict(mod.best, newdata = new_ind, type = "response")

        ## 2.5. Compute indicators of model accuracy in LOOCV:
        res_cv <- tenFoldCV.glm(dat.glm = ref, mod = mod.best,
                                conf_level = conf_level)
        perf_cv <- res_cv$SuccessRate
        indet_cv <- res_cv$IndetRate

        ## 2.6. Return results:
        return(list(PredictedSex = finalSE(postprob, conf_level),
                    PostProb = postprob,
                    BestModel = mod.best,
                    VariablesUsed = predicteurs,
                    cvRate = perf_cv,
                    cvIndet = indet_cv))
    } else { # if no data
        return(list(PredictedSex = NA, PostProb = NA, BestModel = NULL,
                    VariablesUsed = NULL, cvRate = NA, cvIndet = NA))
    }
}
    runs-on: server <- shinyServer(function(input, output, session) {

    myenvg <- new.env() # private environment for PELVIS; will contain the dataset loaded through the UI
    data(refDataBruzek02, package = "PELVIS", envir = myenvg) # load the learning dataset
    refDataBruzek02 <- get("refDataBruzek02", envir = myenvg)

###########################
### 1. TAB "MANUAL EDITING"
    ## 1.1. Initialize the dataframe of results that will be returned through the UI:
    ## (initialized to a matrix with 0 row and 21 columns adequately labeled)
    dat <- matrix(NA, ncol = 21, nrow = 1)
    colnames(dat) <- c("PrSu1", "PrSu2", "PrSu3", "PrSu",
                       "GrSN1", "GrSN2", "GrSN3", "GrSN", "CArc",
                       "InfP1", "InfP2", "InfP3", "InfP", "IsPu",
                       "Sex estimate (Bruzek 2002)",
                       "Statistical sex estimate (2019)",
                       "Prob(F)", "Prob(M)", " Selected predictors in LR model",
                       "10-fold CV accuracy (%)", "Indet. rate in CV (%)")
    dat <- dat[-1, ]
    dat <- as.matrix(dat)
    assign("dat", dat, myenvg) # send this matrix to the private environment

    ## 1.2. Help buttons:
    observeEvent(input$helpPreauri, {
        showModal(modalDialog(
            title = "Preauricular surface",
            img(src = '/style/preauricularSurface.png', align = "left"),
            helpText("as, auricular surface; sn, sciaticnotch; t, piriform tuubercle; pg, preauricular groove; f, female condition; m, male condition."),
            helpText("F: Specific female shape (f-f-f), showing the deep depression, with closed circumference (true preauricular groove), associated with lack of tubercle."),
            helpText("M: Typical male form (m-m-m), showing the relief as smooth or very slight, with open border, associated with presence of tubercle."),
            helpText("Two examples of other female forms: f-f-i, deep depression, pits or groove with closed border, with intermediate aspects of positive relief; i-f-f, little depression with closed border, associated with lack of tubercle."),
            helpText("Two examples of other male forms: m-m-i, little groove opened laterally, with intermediate aspect of positive relief (paraglenoid groove); m-f-m, smooth relief with closed circumference and very prominent tubercle."),
            easyClose = TRUE,
            size = "l"
        ))
    })

    observeEvent(input$helpGSN, {
        showModal(modalDialog(
            title = "Greater sciatic notch",
            img(src = '/style/greaterSN.png', align = "left"),
            helpText("as, auricular surface; sn, sciatic notch; A, top of piriform tubercle, in the case of its absence; A', top of posterior inferior iliac spine; AB and A'B, sciatic notch breadth; CD, sciatic notch depth; AC and A'C, posterior chord of sciatic notch breadth; CB, anterior chord of sciatic notch breadth; AP (A'P), perpendicular at point A (A') to the line formed by the sciatic notch breadth."),
            helpText("F: Extreme female form (f-f-f), showing posterior chord segment AC (or A'C) longer (or close to equality) than the anterior chord CB, symmetry of notch contour associated with no crossing of the line A-P (A'-P) with the contour of the posterior chord."),
            helpText("M: Extreme male form (m-m-m), showing the posterior chord segment AC (A'C) shorter than the anterior chord CB, asymmetry of the outline chords associated with the crossing the line A-P (A'P) with the contour of the posterior chord."),
            helpText("Two examples of other female forms: f-i-f, posterior chord segment AC equal the anterior chord CB, associated with intermediate pattern of the outline chords relative to the deep of the notch, and outline of the posterior chord does not cross the perpendicular line AP. f-f-i, posterior chord segment AC equal to anterior chord CB, presence of relative symmetry of outline chords to the deep of notch and the line AP tangent to the outline of the posterior chord."),
            helpText("Two examples of other male forms: m-m-i, posterior chord segment AC shorter than anterior chord CB, associated with asymmetry of outline chords and line AP tangent to the outline of the posterior chord; m-m-f, posterior chord segment AC shorter than anterior chord CB, associated with asymmetry of outline chords; finally, outline of posterior chord doe not cross perpendicular line AP."),
            easyClose = TRUE,
            size = "l"
        ))
    })

    observeEvent(input$helpCompositeArch, {
        showModal(modalDialog(
            title = "Composite arch",
            img(src = '/style/compositeArch.png', align = "left"),
            helpText("Outline of anterior sciatic notch chord (2), relative to outline of anterior segment of auricular surface (1)."),
            helpText("s, auricular surface; sn, sciatic notch."),
            helpText("M: Absence of composite arch (single curve, 1 = 2)."),
            helpText("F: Presence of composite arch (double curve, 1 \u2260 2)."),
            easyClose = TRUE,
            size = "l"
        ))
    })

    observeEvent(input$helpInfPelvis, {
        showModal(modalDialog(
            title = "Inferior pelvis",
            img(src = '/style/inferiorPelvis.png', align = "left"),
            helpText("pus, pubic symphysis; itu, ischial tuberosity; cp, phallic ridge. Horizontal lines limit the middle part of the ischiopubic ramus. Broken line represents major axis of this structure."),
            helpText("F: Typical female form (f-f-f), showing external eversion of ischiopubic ramus and absence of the phallic ridge, associated with gracility of the bones."),
            helpText("M: Typical male form (m-m-m), showing direct course of medial part of ischiopubic ramus and presence of phallic ridge, associated with robustness of the bones."),
            helpText("Two examples of other female forms: f-f-i, external eversion of ischiopubic ramus and absence of phallic ridge, associated with intermediate aspect of ischiopubic ramus; f-i-f, external eversion of ischiopubic ramus and doubt about absence of crista phallica, associated with gracility of the bones."),
            helpText("Two examples of other male forms: m-i-m, direct course of medial part of ischiopubic ramus and doubt about presence of phallic ridge, associated with robustness of bones; m-m-i, direct course of medial part of ischiopubic ramus and presence of phallic ridge, associated with intermediate aspect of ramus morphology."),
            easyClose = TRUE,
            size = "l"
        ))
    })

    observeEvent(input$helpIschiopubicProp, {
        showModal(modalDialog(
            title = "Ischiopubic proportions",
            img(src = '/style/ischiopubicProp.png', align = "left"),
            helpText("Proportions of length of pubis (pu) and ischium (is)."),
            helpText("M: Male morphology (pu < is)."),
            helpText("F: Female morphology (pu > is)."),
            easyClose = TRUE,
            size = "l"
        ))
    })

    ## 1.3. Compute the table of results
    dat <- eventReactive(input$calcButton, {
        ## We begin here the computation of the table of results;
        ## the computation is done when the user clicks on the button "Apply".
        ## We retrieve here the values entered by the user through the UI:
        valeurs <- c(input$preauriSurf1, input$preauriSurf2, input$preauriSurf3,
                     input$greatSN1, input$greatSN2, input$greatSN3,
                     input$compoArch,
                     input$infPelvis1, input$infPelvis2, input$infPelvis3,
                     input$ispubProp)
        ## Then we get the current state of the table of results:
        temp <- get("dat", envir = myenvg)
        ## We compute the "main characters":
        PaS <- metavar(c(input$preauriSurf1, input$preauriSurf2, input$preauriSurf3))
        GSN <- metavar(c(input$greatSN1, input$greatSN2, input$greatSN3))
        InP <- metavar(c(input$infPelvis1, input$infPelvis2, input$infPelvis3))
        ## Then we can add store the complete values for the current individual:
        traitsInd <- data.frame(PrSu1 = input$preauriSurf1,
                                PrSu2 = input$preauriSurf2,
                                PrSu3 = input$preauriSurf3,
                                GrSN1 = input$greatSN1,
                                GrSN2 = input$greatSN2,
                                GrSN3 = input$greatSN3,
                                CArc = input$compoArch,
                                InfP1 = input$infPelvis1,
                                InfP2 = input$infPelvis2,
                                InfP3 = input$infPelvis3,
                                IsPu = input$ispubProp)
        ## Mark the columns corresponding to nonmissing (i.e., observed) traits:
        colOk <- colnames(traitsInd)[which(traitsInd[1,]!="NA")]
        traitsInd <- data.frame(traitsInd[ , traitsInd[1,]!="NA"])
        colnames(traitsInd) <- colOk
        ## And finally, compute a sex estimate for the current individual:
        resStatEstimateME <- indiv_sexing(ref = refDataBruzek02,
                                          new_ind = traitsInd,
                                          strategy = input$indivSelvar,
                                          conf_level = as.numeric(input$indivConf))
        ## Add all those informations in the table of results:
        temp <- rbind(temp, # i.e., we add a new row below the existing results
                      c(input$preauriSurf1, input$preauriSurf2, input$preauriSurf3, PaS,
                        input$greatSN1, input$greatSN2, input$greatSN3, GSN,
                        input$compoArch,
                        input$infPelvis1, input$infPelvis2, input$infPelvis3, InP,
                        input$ispubProp,
                        bruzek02(c(PaS, GSN, InP, input$compoArch, input$ispubProp)),
                        resStatEstimateME$PredictedSex, # sex estimate with Bruzek 2019
                        round(1 - resStatEstimateME$PostProb, 3), # p(F) with Bruzek 2019
                        round(resStatEstimateME$PostProb, 3), # p(M) with Bruzek 2019
                        resStatEstimateME$VariablesUsed, # traits in LR model
                        round(100 * resStatEstimateME$cvRate, 2), # success rate in CV
                        round(100 * resStatEstimateME$cvIndet,2) # indet rate in CV
                        )
                      )
        rownames(temp)[input$calcButton] <- input$indivName # add a proper row name
        ## We can now update the current state of the table of results, with this new row:
        assign("dat", temp, envir = myenvg)
        return(temp) # pass this new table to renderDT (below) for fisplay in the UI
  })

    ## 1.4. Display the table of results
    output$tabResME <- DT::renderDT(dat(),
                                    options = list(columnDefs = list(list(className = 'dt-center', targets = "_all")))
                                    )

    ## 1.5 Render and display a download button
    ## (rendered only when a first indiv has been submitted)
    output$button_download_resultsME <- renderUI({
        if (input$calcButton > 0) { 
            downloadButton("download_resultsME", "Download the complete table [CSV file]")
        } else { 
            return()
        }
    })
    output$download_resultsME <- downloadHandler(filename = "results_SexingOsCoxae.csv",
                                                 content = function(file) {
                                                     write.csv(dat(), file)
                                                 })


############################
### 2. TAB "INPUT FROM FILE"
    ## 2.1. Load the data file
    ## When the user clicks on the button:
    observeEvent(input$loadData, {
        if (! is.null(input$file$name)) { # is there really a file?
            datUser <- read.table(input$file$datapath, header = TRUE,
                                  stringsAsFactors = TRUE,
                                  sep = input$fieldSep, na.strings = input$charNA)
            ## Handling rownames:
            if (input$rowNames) { # are there row names?
                if (any(duplicated(datUser[ , 1]))) { # if there are duplicates: invalid row names
                    showModal(modalDialog(
                        title = "Invalid row names",
                        helpText("There are duplicates in the first column; it cannot be used as row names. Arbitrary row names have been given."),
                        easyClose = TRUE,
                        size = "l"
                    ))
                    rownames(datUser) <- 1:nrow(datUser)
                } else { # there are no duplicates: valid row names!
                    rownames(datUser) <- datUser[ , 1] # add them to the dataframe
                    datUser[ , 1] <- NULL # and then remove them as a variable
                }
            } else { # no rownames
                rownames(datUser) <- 1:nrow(datUser) # just add an individual number (useful for the progress bar)
            }
            ## Is the dataframe valid?
            if (valid_data(datUser)) {
                assign("datUser", datUser, envir = myenvg) # store in the private env
            } else {
                showModal(modalDialog(title = "Error",
                                      "Invalid file. Please check the field separator and the spelling of column names.",
                                      easyClose = TRUE)
                          )
            }
        } else { # there is no file!
            showModal(modalDialog(title = "Error",
                                  "Please select a file on your computer.",
                                  easyClose = TRUE)
                      )
        }
    })

    ## 2.2. Compute sex estimates
    output$tabResFF <- DT::renderDT({
        ## 2.2.1. Progress bar
        ## Initialize a progress bar:
        progressFF <- shiny::Progress$new()
        progressFF$set(message = "Computing results...", value = 0)
        ## Close the progress bar when the computation is done:                        
        on.exit(progressFF$close())
        ## Function for the progress bar:
        updateProgressBarFF <- function(currentValue = NULL, detail = NULL, total = NULL) {
            if (is.null(currentValue)) {
                currentValue <- progressFF$getValue()
                currentValue <- currentValue + 1 / total
            }
            progressFF$set(value = currentValue, detail = detail)
        }
        ## 2.2.2. Return the dataframe:
        if (input$loadData > 0 & exists("datUser", envir = myenvg)) { # file OK
            finalResultFF <- dataframe_sexing(ref = refDataBruzek02,
                                              data = get("datUser", envir = myenvg),
                                              updateProgressBarFF,
                                              strategy = input$fileSelvar,
                                              conf_level = as.numeric(input$fileConf))
            assign("ResultsFF", finalResultFF, envir = myenvg)
            return(finalResultFF)
        } else { # file not OK
            NULL # then return nothing
        }
    }, options = list(columnDefs = list(list(className = 'dt-center', targets = "_all")))
    )

    ## 2.2.3. Download button
    output$button_download_resultsFF <- renderUI({
        if (input$loadData>0 & exists("datUser", envir = myenvg)) { 
            downloadButton("download_resultsFF", "Download the complete table [CSV file]")
        } else {
            return()
        }
    })
    output$download_resultsFF <- downloadHandler(filename = "results_SexingOsCoxae.csv",
                                                 content = function(file) {
                                                     saveData <- get("ResultsFF", finalResultFF, envir = myenvg)
                                                     write.csv(saveData, file)
                                                 })
})
    steps: metavar <- function(x, threshold = 2) {
### Internal function
### x: character vector, with possible values "f", "i", "m" or NA.
### threshold: threshold for the majority rule; set to 2 by default since x is supposed to be of length 3 (cf. Bruzek 2002)
### output -> a main character value, "F", "0" or "M", based on a majority rule.

    if (sum(is.na(x)) >= threshold) { # If there are too many missing values,
        return(NA)
    } else if (sum(x == "NA", na.rm = TRUE) >= threshold) {
        return(NA)
    } else if (sum(x == "f", na.rm = TRUE) >= threshold) { # Majority of "f"
        return("F")
    } else if (sum(x == "m", na.rm = TRUE) >= threshold) { # Majority of "m"
        return("M")
    } else { # No clear majority
        return("0")
    }
}

addMetavars <- function(dat) {
    .Deprecated("add_metavars")
    add_metavars(dat)
}

add_metavars <- function(dat) {
### dat: dataframe included the observed values for the eleven trichotomic traits (PrSu1, PrSu2, etc.)
### output -> same dataframe, with three more columns corresponding to the main characters (PrSu, GrSN, InfP)

    dat$PrSu <- factor(apply(dat[ , c("PrSu1", "PrSu2", "PrSu3")], MARGIN = 1, FUN = metavar))
    dat$GrSN <- factor(apply(dat[ , c("GrSN1", "GrSN2", "GrSN3")], MARGIN = 1, FUN = metavar))
    dat$InfP <- factor(apply(dat[ , c("InfP1", "InfP2", "InfP3")], MARGIN = 1, FUN = metavar))

    return(dat)
}

finalSE <- function(x, threshold = 0.95) {
### x: vector of posterior probabilities of being a male individual
### threshold: decision threshold to produce a sex estimate (otherwise indet.)
    det <- ifelse(x >= threshold, "M", ifelse(x <= 1-threshold, "F", "I"))
    return(det)
}
      - name: start_pelvis <- function() {
    ## Define the UI and server files for the app:
    app <- shiny::shinyApp(ui = ui, server = server)
    ## Define a folder that contains a CSS sheet and images:
    shiny::addResourcePath(prefix = "style", directoryPath = system.file("deco", package = "PELVIS"))
    ## Run the app:
    shiny::runApp(app)
}

StartPELVIS <- function() {
    .Deprecated("start_pelvis")
    start_pelvis()
}
        uses: tenFoldCV.glm <- function(dat.glm, mod, conf_level = 0.95) {
### dat.glm: dataframe on which to perform a 10-fold CV (with no missing values)
### mod: GLM to evaluate
### conf_level: 0.95 by default, confidence level needed to produce a sex estimate

### Prepare constants and data:
    N <- nrow(dat.glm) # sample size in the dataset
    ## Reorder the rows to introduce some randomness:
    dat.glm <- dat.glm[sample(1:N, size = N, replace = FALSE) , ]

######################################################
### 1. Prepare and initialize a dataframe of results #
######################################################
    MatRes <- matrix(nrow = N, ncol = 3)
    colnames(MatRes) <- c("True_sex", "Proba_10CV", "Predict_sex_10CV")
    MatRes <- data.frame(MatRes)
    MatRes[ , 1] <- as.character(dat.glm$Sex)

#############################
### 2. Perform a 10-fold CV #
#############################
    for (k in 1:10) { # for each of the ten folds ("splits", "folds"),
        pioch <- (1:N <= k*N/10) & (1:N > (k-1)*N/10) # draw at random 10% of individuals as a test sample
        app <- dat.glm[!pioch, ] # and train a GLM on the remaining 90%.
        val <- dat.glm[pioch, ]
        modTemp <- glm(mod$call$formula, family = binomial, data = app)
        MatRes[pioch, 2] <- predict(modTemp, newdata = val, type = "response")
    }
    MatRes[ , 3] <- finalSE(MatRes[ , 2], conf_level)

#######################
### 3. Model accuracy #
#######################
    indet_rate <- nrow(MatRes[MatRes$Predict_sex_10CV == "I", ]) / nrow(MatRes)
    if (indet_rate < 1) { 
        success_rate <- nrow(MatRes[(MatRes$Predict_sex_10CV=="F" & MatRes$True_sex=="F") | (MatRes$Predict_sex_10CV=="M" & MatRes$True_sex=="M"), ]) / nrow(MatRes[MatRes$Predict_sex_10CV!="I", ])
    } else { # if there is only indeterminate individuals (might happen if one single trait is used as predictor)
        success_rate <- NA
    }

###########################
### 4. Return the results #
###########################
    return(list(ClassifResults = MatRes, IndetRate = indet_rate, SuccessRate = success_rate))
}

      - name: ui <- shinyUI(fluidPage(theme = "/style/kappa.css",

    tags$style(HTML("
    .btn-help.btn {
      display: inline-block;
      padding: 0px 5px 0px 5px;
      border-radius: 20px;
      font-size: 0.8em;
      margin: 0 0 0 0;
      vertical-align: middle;
      color: gray;
      font-weight: bold;
      background-color: white;
      border-color: gray;
    }
    .btn-help.btn:hover {
      color: white;
      background-color: #0098B6;
    }
    .btn-help.active {
      color: white;
      background-color: #0098B6;
      border-color: #0098B6;
    }
    .shiny-notification{
      position: fixed;
      top: 60%;
      left: 20%;
      right: 20%;
      border-color: #E23D22;
      color: #0098B6;
    }
    "
    )),

    titlePanel(img(src = "/style/banner_PELVIS.png", height = "40px"),
               windowTitle = "PELVIS"),

    ## Divide the window into two tabs:
    tabsetPanel(

############################
### 1. TAB "MANUAL EDITING":
        tabPanel("Data input: manual editing",
                 verticalLayout(
                     wellPanel(
                         fluidRow(
                             column(4,
                                    textInput("indivName",
                                              label = h4("Name of the unknown individual"),
                                              value = "Indiv01", width = "400px")
                                    ),
                             column(4,
                                    radioButtons("indivSelvar", label = h4("Strategy for variable selection"),
                                                choices = c("None", "AIC", "BIC"),
                                                selected = "BIC", inline = TRUE)
                                    ),
                             column(4,
                                    radioButtons("indivConf", label = h4("Confidence level for a sex estimate"),
                                                 choices = c("90 %" = "0.9", "95 %" = "0.95"),
                                                 selected = "0.95", inline = TRUE)
                                    )
                             ),
                         fluidRow(
                             column(3, # Preauricular surface
                                    p(h5("Preauricular surface (PrSu)"), actionButton("helpPreauri", label = "?", class = "btn-help")),
                                    selectInput("preauriSurf1",
                                                label = "Development of negative relief on preauricular surface  (PrSu1)",
                                                choices = c("Could not be observed" = "NA",
                                                            "f, deep depression well-delimited" = "f",
                                                            "i, intermediate form" = "i",
                                                            "m, relief smooth or very slightly negative relief" = "m"),
                                                selected = "NA", multiple = FALSE),
                                    selectInput("preauriSurf2",
                                                label = "Aspects of grooves or pitting (PrSu2)",
                                                choices = c("Could not be observed" = "NA",
                                                            "f, pits or groove with closed circumference" = "f",
                                                            "i, intermediate form" = "i",
                                                            "m, depression with open circumference" = "m"),
                                                selected = "NA", multiple = FALSE),
                                    selectInput("preauriSurf3",
                                                label = "Development of positive relief on preauricular surface (PrSu3)",
                                                choices = c("Could not be observed" = "NA",
                                                            "f, lack of tubercule" = "f",
                                                            "i, intermediate form" = "i",
                                                            "m, tubercule present or clear protuberance" = "m"),
                                                selected = "NA", multiple = FALSE)
                                    ),
                             column(3, # Great sciatic notch
                                    p(h5("Great sciatic notch (GrSN)"), actionButton("helpGSN", label = "?", class = "btn-help")),
                                    selectInput("greatSN1",
                                                label = "Proportion of length of sciatic chords  (GrSN1)",
                                                choices = c("Could not be observed" = "NA",
                                                            "f, posterior chord segment (AC) longer than or equal to anterior chord (CB)" = "f",
                                                            "i, intermediate form" = "i",
                                                            "m, posterior chord (AC) shorter than anterior chord (CB)" = "m"),
                                                selected = "NA", multiple = FALSE),
                                    selectInput("greatSN2",
                                                label = "Form of contour notch chords (GrSN2)",
                                                choices = c("Could not be observed" = "NA",
                                                            "f, symmetry relative to depth in basal portion of sciatic notch" = "f",
                                                            "i, intermediate form" = "i",
                                                            "m, asymmetry relative to depth of sciatic notch" = "m"),
                                                selected = "NA", multiple = FALSE),
                                    selectInput("greatSN3",
                                                label = "Contour of posterior notch chord relative to line from point A to sciatic notch breadth (GrSN3)",
                                                choices = c("Could not be observed" = "NA",
                                                            "f, outline of posterior chord doesn't cross perpendicular line" = "f",
                                                            "i, intermediate form" = "i",
                                                            "m, contour of posterior chord crosses perpendicular line" = "m"),
                                                selected = "NA", multiple = FALSE)
                                    ),
                             column(2, # Composite arch
                                    p(h5("Composite arch (CArc)"), actionButton("helpCompositeArch", label = "?", class = "btn-help")),
                                    radioButtons("compoArch",
                                                 label = "Relation between outline of sciatic notch and outline of auricular surface",
                                                 choices = c("Could not be observed" = "NA",
                                                             "F, Double curve" = "F",
                                                             "0, Intermediate form" = "0",
                                                             "M, Single curve" = "M"),
                                                 selected = "NA")                
                                    ),
                             column(2, # Inferior pelvis
                                    p(h5("Inferior pelvis (InfP)"), actionButton("helpInfPelvis", label = "?", class = "btn-help")),
                                    selectInput("infPelvis1",
                                                label = "Characterization of margo inferior ossis coxae (InfP1)",
                                                choices = c("Could not be observed" = "NA",
                                                            "f, external eversion" = "f",
                                                            "i, intermediate form" = "i",
                                                            "m, direct course of medial part" = "m"),
                                                selected = "NA", multiple = FALSE),
                                    selectInput("infPelvis2",
                                                label = "Phallic ridge (InfP2)",
                                                choices = c("Could not be observed" = "NA",
                                                            "f, lack of the phallic ridge or presence of only little mound" = "f",
                                                            "i, intermediate form" = "i",
                                                            "m, clear presence of the phallic ridge" = "m"),
                                                selected = "NA", multiple = FALSE),
                                    selectInput("infPelvis3",
                                                label = "Ischio-pubic ramus aspect (InfP3)",
                                                choices = c("Could not be observed" = "NA",
                                                            "f, gracile aspect" = "f",
                                                            "i, intermediate form" = "i",
                                                            "m, robust aspect" = "m"),
                                                selected = "NA", multiple = FALSE)
                                    ),
                             column(2, # Ischiopubic proportion
                                    p(h5("Ischiopubic proportion (IsPu)"), actionButton("helpIschiopubicProp", label = "?", class = "btn-help")),
                                    radioButtons("ispubProp",
                                                 label = "Relation between pubis and ischium lengths",
                                                 choices = c("Could not be observed" = "NA",
                                                             "F, Pubis longer than ischium" = "F",
                                                             "0, Intermediate form" = "0",
                                                             "M, Ischium longer than pubis" = "M"),
                                                 selected = "NA")
                                    )
                         ),
                         actionButton("calcButton", label = "Compute sex estimate", icon = icon("cogs"))
                     ),

                     wellPanel(
                         h3("Results"),
                         helpText("Posterior probabilities are rounded to the third decimal place."),
                         div(DT::DTOutput("tabResME")),
                         uiOutput("button_download_resultsME")
                     )
                 ) # end "verticalLayout"
                 ), # end "tabPanel" for manual editing


#################################
### 2. TAB "INPUT FROM TEXT FILE"
        tabPanel("Data input: from text file",
                 verticalLayout(
                     wellPanel(
                         fluidRow(
                             column(4,
                                    fileInput("file", label = h4("File"), accept = c(".csv", ".txt")),
                                    helpText("Only files in plain-text format (such as CSV or TXT files) are accepted.",
                                             "Please make sure that the headers are correctly defined."),
                                    helpText("The 'row names' option should be checked if and only if each bone has a unique attribute or ID in the first column of the data file. This option is really about bone names, and not individual names: each individual may have two bones in the dataset, so that individual names cannot be seen as unique IDs.")
                                    ),
                             column(4,
                                    selectInput("fieldSep", label = h4("Field separator"),
                                                choices = c("Semicolon (;)" = ";", "Comma(,)" = ",", "Tabulation" = "\t", "Space" = " "),
                                                selected = ";", multiple = FALSE),
                                    textInput("charNA", label = h4("Indicator for missing values"), value = ""),
                                    checkboxInput("rowNames", label = "Row names in first column", value = TRUE)
                                    ),
                             column(4,
                                    radioButtons("fileSelvar", label = h4("Strategy for variable selection"),
                                                 choices = c("None", "AIC", "BIC"),
                                                 selected = "BIC", inline = TRUE),
                                    radioButtons("fileConf", label = h4("Confidence level for a sex estimate"),
                                                 choices = c("90 %" = "0.9", "95 %" = "0.95"),
                                                 selected = "0.95", inline = TRUE)
                                    )
                         ),
                         ## server.r waits for a click on this button:
                         actionButton("loadData", "Load dataset", icon = icon("file-upload")),
                         helpText("Please note that for large input files with many missing values, the calculation may be slow due to the stepwise procedure of variable selection in logistic regression models (if this option is selected), and the calculation of 10-fold cross-validation success rate for each model.")
                        ),
                         wellPanel(
                             h3("Results"),
                             helpText("Posterior probabilities are rounded to the third decimal place."),
                             div(DT::DTOutput("tabResFF")),
                             uiOutput("button_download_resultsFF")
                         )
                     )
                 )
                 ) # end "tabsetPanel"
    ))
        run: valid_data <- function(data) {
### Checks if a dataframe "data" is suitable for PELVIS
### output -> boolean, TRUE/FALSE

    ## If there are not enough columns (i.e., less than the 11 required traits):
    if (ncol(data) < 11) {
        warning("The dataset does not have enough columns: at least 11 are required.")
        return(FALSE)
    } else {
        ## If all traits are OK:
        if (all(c("PrSu1", "PrSu2", "PrSu3", "GrSN1", "GrSN2", "GrSN3",
                  "CArc", "InfP1", "InfP2", "InfP3", "IsPu") %in% colnames(data))) {
            return(TRUE)
        } else {
            warning("Invalid column names.")
            return(FALSE)
        }
    }
}

      - name: 
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
        run: ./#pragma once

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
};                                        # Update this to build your Android or iOS application

      - name: 
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
        uses: #pragma once

#include "../Command.h"

// Sends a quit signal to the session.
// No params.
class QuitCommand : public Command
{
public:
    QuitCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};
        with: 
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
          appknox_access_token: #pragma once

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
          file_path: 
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
}       # Specify the path to your .ipa or .apk here
          risk_threshold: #pragma once

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
};                                     # Update this to desired risk threshold [LOW, MEDIUM, HIGH, CRITICAL]
          sarif: #pragma once

struct Result
{
    bool error;
    bool quit;
};

      - name: import sys

print("")
print("..............CONSIGNES.................")
print("  Ce script va calculer votre moyenne pour vous!")
print("Il faudra lui spécifier le nom de la matiere et le nombre de notes que vous avez eu pour cette matière")
print("Ne taper que des nombres")
print("..........................................")
print("")
print("")
print("Voici le choix de la matière :")
print("{1} Maths")
print("{2} Français")
print("{3} Anglais")
print("{4} Espanol")
print("{5} Sport")
print("{6} Technologie")
print("{7} Physique")
print("{8} SVT")
print("{9} Histoire-Géo")
print("{10} Religion")
try:
        choice = int(input("Quel matière choisissez-vous?"))
except ValueError:
        print("Vous n'avez pas taper un nombre!")
        print("Au revoir...")
        input("Taper la touche Entrée pour quitter")
        sys.exit(1)
else:
        if choice == 1:
                choice = "Maths"
        elif choice == 2:
                choice = "Français"
        elif choice == 3:
                choice = "Anglais"
        elif choice == 4:
                choice = "Espagnol"
        elif choice == 5:
                choice = "Sport"
        elif choice == 6:
                choice = "Technologie"
        elif choice == 7:
                choice = "Physique"
        elif choice == 8:
                choice = "SVT"
        elif choice == 9:
                choice = "Histoire-Géo"
        elif choice == 10:
                choice = "Religion"
        else:
                print("Ceci ne correspond pas aux matières proposées!")
                print("Au revoir")
                input("Taper la touche Entrée pour quitter")
                sys.exit(1)
        print("Vous avez choisi de calculer votre moyenne pour:", choice)
        print("")
        print("..........................................")
        print("")
        try:
                nbnot = int(input("Combien avez vous eu de notes ?"))
        except ValueError:
                print("Vous n'avez pas taper un nombre!")
                print("Au revoir...")
                input("Taper la touche Entrée pour quitter")
                sys.exit(1)
        else:
                if nbnot == 1 or nbnot >= 50:
                        print("Ce nombre de note est incorrect!")
                        input("Appuyer sur la touche Entrée pour quitter")
                        sys.exit(1)
                note = []
                coef = []
                print("Appuyer sur la touche Entrée après chaque notes")
                for i in range(nbnot):
                        try:
                                i =  float(input("Taper une note: "))
                                p = float(input("Quel est son coefficient?"))
                        except ValueError:
                                print("Vous n'avez pas taper un nombre!")
                                print("Au revoir...")
                                input("Taper la touche Entrée pour quitter")
                                sys.exit(1)
                        else:
                                coef.append(p)
                                i = i * p
                                note.append(i)
                add = sum(note)#sum() additionne les valeurs de note entre elles
                ad = sum(coef)# addtions des coefficients
                calc = add/ad
# Pour calculer une moyenne:
# additionner les notes
# additionner les coefficients
# diviser le premier résultat avec le deuxième
                if round(calc) == calc:
                        calc = int(calc)
                else:
                        calc = float(calc)
                print("")
                print("[..........  ...........................]")
                print("Votre moyenne en", choice,"est de", float(round(calc, 2)))
                print("[.......................................]")
                print("")
                print("")
                input("Appuyer sur la touche Entrée pour quitter")
        if: import java.util.Scanner;

public class matutil {
        public static void main(String[] args) {
                Scanner reader = new Scanner(System.in);
                System.out.println("Taper un nombre: ");
                double nb = reader.nextDouble();
            System.out.println("................................................................");
                for (int i=1; i <= nb+1; i++) {
                if (nb/i == Math.round(nb/i)){
                        System.out.println(nb + " est divisible par " + i);
                    System.out.println(nb + " divise par " + i + " vaut " + nb/i);
                System.out.println(" ");
                } else {
                    continue; }
        }
            for (int x = 1; x <= 28; x++) {
                System.out.println(x + " fois " + nb + "= " + x*nb);
    }
    System.out.println(" ");
    System.out.println("Carre: " + nb*nb);
    System.out.println(" ");
    if (Math.sqrt(nb) == Math.round(Math.sqrt(nb))){
        System.out.println("La racine carree de " + nb + " tombe juste");
        System.out.println(" ");
    } else{
        System.out.println("La racine carree de "  + nb + " ne tombe pas juste");
            System.out.println("Racine carree: " + Math.sqrt(nb));
        System.out.println(" ");
    }
    for (int v=1; v <= 11; v++) {
        System.out.println("Exposant " + v +": " + Math.pow(nb, v));
        }
        }
}
        uses: import sys
from math import sqrt


def main():
    for i in range(1, nb+1):
        if nb/i == round(nb/i):
            print(nb, "est divisible par", i,)
            print(nb, "divisé par", i, "vaut", nb/i)
        else:
            pass
    for x in range(1, 21):
        print(x, "×", nb, "=", x*nb)
    print("Carré:", nb*nb)
    if sqrt(nb) == round(sqrt(nb)):
        print("La racine carrée de", nb, "tombe juste")
    else:
        print("La racine carrée de" , nb, "ne tombe pas juste")
    print("Racine carrée:", round(sqrt(nb)))
    for v in range(1,11):
        print("Exposant",v,":", nb ** v)
        v += 1

try:
    nb = float(input("Taper un nombre =>"))
    print("......................................")
except ValueError:
    print("Vous n'avez pas tapé un nombre")
    sys.exit()
else:
    if nb == int(nb):
        nb = int(nb)
        main()
    else:
        nb = float(nb)
        main()
        with: import sys


nbpremiers = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 43, 59, 61, 67, 71]
print("PGCD: Plus Grand Diviseur Commun")

def Euclide(a, b):
        while a%b != 0:
                a, b = b, a%b
                maxi = max(a, b)
                mini = min(a, b)
                reste = maxi%mini
                div = int(maxi/mini)
                print("{0} = {1}*{2} + {3}".format(maxi, mini, div, reste))
                maxi = mini
                mini = reste
        Back()

def Back():
        print("................................................................................")
        print()
        print("1- Revenir au menu principal")
        print("2- Quitter")
        print()
        try:
                re = int(input("Taper le nombre de votre choix => "))
        except ValueError:
                print("Vous n'avez pas taper un nombre valable")
                sys.exit()
        if re == 1:
                Menu()
        elif re == 2:
                print("Au revoir.")
                sys.exit()
        else:
                print("Ce nombre ne correspond pas aux choix disponibles")
                sys.exit()

def PGCD(A, B):
        if A <= 0 or B <= 0:
                print("Les nombres ne peuvent pas être égaux ou inférieur à 0")
                sys.exit()
        elif A >= 100000000 or B >= 100000000:
                print("Les nombres ne peuvent pas être au-dessus de 1000")
                sys.exit()
        divA = []
        divB = []
        divcom = []
        for x in range(1, A+1):
                calc = int(A)/x
                if calc == int(round(calc)):
                        if x in divA:
                                pass
                        else:
                                divA.append(int(x))
                else:
                        pass
        for i in range(1,B+1):
                ca = int(B)/i
                if ca == int(round(ca)):
                        if i in divB:
                                pass
                        else:
                                divB.append(i)
                else:
                        pass
        for nb in range(max(A, B)+1):
                if nb in divA and nb in divB:
                        if nb in divcom:
                                pass
                        else:
                                divcom.append(nb)
                else:
                        pass
        print("................................................................................")
        print("Liste des diviseurs de", A)
        print(divA)
        print("")
        print("Liste des diviseurs de", B)
        print(divB)
        print("")
        print("Liste des diviseurs communs")
        print(divcom)
        longueur = max(divcom)
        print("D'où le plus grand diviseur commun de {0} et de {1} est:".format(A, B), longueur)
        Back()

def View(a, b):
        if a != int(a) and b != int(b):
                print("Tape des entiers!")
                sys.exit()
        if a > b:
                grand = a
                g = b
        elif b > a:
                grand = b
                g = a
        print("PGCD({0}, {1})".format(a, b))
        while True:
                petit = max(a, b) - min(a, b)
                print("= PGCD({0}, {1})".format(min(a, b), petit))
                ok = min(a, b)
                encor = max(ok, petit) - min(petit, ok)
                print("= PGCD({0}, {1})".format(petit, encor))
                a = petit
                b = encor
                if petit == encor:
                        break
                if max(a, b) <= 0 or min(a, b) <= 0 or petit <= 0:
                        break
        Back()

def Menu():
        print()
        print()
        print("......................................MENU......................................")
        print()
        print("1- Methode d'Euclide")
        print("2- Methode par soustractions successives")
        print("3- Methode détaillée")
        print()
        try:
                choix = int(input("Taper le nombre correspondant à votre choix => "))
        except ValueError:
                print("Vous n'avez pas taper un nombre valable")
                sys.exit()
        print("..................................................................................")
        try:
                if choix == 1:
                        a = int(input("Taper A: "))
                        b = int(input("Taper B: "))
                        Euclide(a, b)
                elif choix == 2:
                        a = int(input("Taper A: "))
                        b = int(input("Taper B: "))
                        View(a, b)
                elif choix == 3:
                        A = int(input("Taper A: "))
                        B = int(input("Taper B: "))
                        PGCD(A, B)
                else:
                        print("Ce nombre ne correspond pas aux choix disponibles")
        except ValueError:
                print("Vous n'avez pas taper un nombre valable")
                sys.exit()
Menu()
          sarif_file: u = {} # {0:1, 1:2, 2:5, 3:26,...}


def recurrence_sequence(first_n: int, first_u_n: int, n_seeked: int):
    """
    Parameters: first_n corresponds to the value of the initial term of the sequence whose image is known by Un.
                first_u_n is the image of this initial term by Un
                n_seeked is the value n searched for
    Example: U0 = 1
              0 would then be the value of the "first_n" parameter, and
              1 the value of "first_u_n".
    """
    for n in range(first_n, n_seeked+1):
        u[n] = u[n-1]*2 - 3 if n-1 in u.keys() else first_u_n
        yield f"U{n} = {u[n]}"


for i in recurrence_sequence(0, 1, int(input("n value seeked: "))):
    print(i)
