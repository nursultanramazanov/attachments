# This workflow uses actions that are not certified by GitHub.  They are
# provided by a third-party and are governed by separate terms of service,
# privacy policy, and support documentation.
#
# This workflow will install a prebuilt Ruby version, install dependencies, and
# run tests and linters.
name: "Ruby on Rails CI"
on: FROM rocker/verse

ARG R_VERSION
ARG BUILD_DATE
ARG CRAN
ENV BUILD_DATE ${BUILD_DATE:-2020-05-20}
ENV R_VERSION=${R_VERSION:-3.6.3} \
    CRAN=${CRAN:-https://cran.rstudio.com} \ 
    TERM=xterm

# Set the locale
#RUN sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen && \
#    locale-gen
ENV LANG en_US.UTF-8  
ENV LANGUAGE en_US:en  
ENV LC_ALL en_US.UTF-8   


RUN useradd docker \
        && mkdir /home/docker \
        && chown docker:docker /home/docker \
        && addgroup docker staff

RUN  apt-get update \
        && DEBIAN_FRONTEND="noninteractive" apt-get install -y --no-install-recommends \
  apt-utils \
  pandoc-citeproc lmodern

RUN R -e 'BiocManager::install("Icens")'

RUN install2.r --error \
    magrittr \ 
    interval \ 
    cgam \ 
    icenReg \ 
    km.ci \ 
    flexsurv \ 
    caret \ 
    tableone \ 
    PropCIs

WORKDIR /n95_refit

RUN git clone https://github.com/cryanking/n95_refit.git /n95_refit

RUN R -e 'rmarkdown::render("/n95_refit/n95_report.Rmd")' 



  push: Sex,Title,Respirator,Days Worn,Sterilizations,Uses,Uses per day,Fits well,Mask quality,Qualitative Fit
Male,Resident,3M 1804 Vflex,1,0,6,6,Yes,Like New,Fail
Male,Attending,3M 1804 Vflex,2,1,10,5,Yes,Like New,Pass
Female,Attending,3M 1804 Vflex,3,1,9,3,Yes,Like New,Fail
Female,Attending,3M 1804 Vflex,3,1,9,3,Yes,Like New,Pass
Male,Resident,3M 1804 Vflex,3,1,6,2,Yes,Like New,Pass
Female,CRNA,3M 1804 Vflex,3,0,9,3,Yes,Like New,Pass
Female,CRNA,3M 1804 Vflex,4,1,8,2,Yes,Like New,Fail
Female,CRNA,3M 1804 Vflex,4,1,6,1.5,Yes,Like New,Fail
Female,CRNA,3M 1804 Vflex,4,0,9,2.25,No,Good,Fail
Male,Attending,3M 1804 Vflex,4,0,60,15,Yes,Like New,Pass
Male,Attending,3M 1804 Vflex,4,0,60,15,Yes,Like New,Pass
Male,Attending,3M 1804 Vflex,4,0,10,2.5,Yes,Like New,Pass
Female,Resident,3M 1804 Vflex,4,0,20,5,Yes,Like New,Pass
Male,Resident,3M 1804 Vflex,5,2,20,4,Yes,Good,Fail
Female,CRNA,3M 1804 Vflex,5,1,10,2,No,Good,Fail
Female,Resident,3M 1804 Vflex,5,0,30,6,Yes,Like New,Fail
Female,CRNA,3M 1804 Vflex,5,1,12,2.4,Yes,Good,Fail
Female,CRNA,3M 1804 Vflex,5,1,10,2,No,Poor,Fail
Female,CRNA,3M 1804 Vflex,5,2,10,2,Yes,Like New,Fail
Female,CRNA,3M 1804 Vflex,5,1,12,2.4,Yes,Like New,Fail
Male,Resident,3M 1804 Vflex,5,0,20,4,Yes,Like New,Pass
Male,CRNA,3M 1804 Vflex,5,1,20,4,Yes,Like New,Pass
Male,CRNA,3M 1804 Vflex,5,0,16,3.2,Yes,Good,Pass
Female,CRNA,3M 1804 Vflex,6,1,18,3,Yes,Good,Fail
Female,CRNA,3M 1804 Vflex,6,0,12,2,Yes,Like New,Fail
Male,CRNA,3M 1804 Vflex,6,0,18,3,Yes,Like New,Pass
Female,Resident,3M 1804 Vflex,6,0,6,1,Yes,Like New,Pass
Female,Resident,3M 1804 Vflex,6,3,30,5,Yes,Good,Pass
Female,CRNA,3M 1804 Vflex,6,1,16,2.66666666666667,No,Like New,Pass
Female,CRNA,3m 1860 Teal,6,0,12,2,Yes,Like New,Pass
Female,CRNA,3M 1804 Vflex,7,0,15,2.14285714285714,Yes,Good,Fail
Female,CRNA,3M 1804 Vflex,7,0,15,2.14285714285714,Yes,Good,Fail
Male,Resident,3M 1804 Vflex,7,0,14,2,Yes,Good,Fail
Female,CRNA,3M 1804 Vflex,7,2,28,4,Yes,Poor,Fail
Male,Attending,3M 1804 Vflex,7,1,40,5.71428571428571,Yes,Like New,Pass
Male,CRNA,3M 1804 Vflex,7,0,20,2.85714285714286,Yes,Like New,Pass
Female,Resident,3M 1804 Vflex,7,0,15,2.14285714285714,Yes,Like New,Pass
Female,CRNA,3M 1804 Vflex,8,1,32,4,Yes,Good,Fail
Male,CRNA,3M 1804 Vflex,8,3,24,3,Yes,Good,Pass
Female,CRNA,3M 1804 Vflex,9,0,40,4.44444444444444,No,Good,Fail
Male,Attending,3M 1804 Vflex,10,0,60,6,Yes,Good,Fail
Male,CRNA,3M 1804 Vflex,10,1,15,1.5,Yes,Like New,Fail
Male,Resident,3M 1804 Vflex,10,1,36,3.6,Yes,Good,Pass
Female,Attending,3M 1804 Vflex,10,0,25,2.5,Yes,Good,Pass
Female,Fellow,3M 1804 Vflex,10,0,20,2,Yes,Like New,Pass
Male,CRNA,3M 1804 Vflex,10,0,10,1,Yes,Like New,Pass
Male,CRNA,3M 1804 Vflex,10,1,15,1.5,Yes,Like New,Pass
Female,Attending,3m 1860 Teal,10,1,60,6,Yes,Like New,Pass
Female,CRNA,3m 1860 Teal,10,1,60,6,Yes,Poor,Pass
Male,Attending,3M 1804 Vflex,11,2,40,3.63636363636364,Yes,Good,Fail
Male,Resident,3M 1804 Vflex,11,2,15,1.36363636363636,No,Good,Fail
Female,Attending,3M 1804 Vflex,12,1,60,5,No,Good,Fail
Female,CRNA,3M 1804 Vflex,12,1,15,1.25,No,Good,Fail
Male,CRNA,3M 1804 Vflex,12,1,40,3.33333333333333,Yes,Good,Fail
Female,CRNA,3M 1804 Vflex,12,1,15,1.25,Yes,Good,Fail
Female,Attending,3M 1804 Vflex,12,1,50,4.16666666666667,Yes,Good,Pass
Male,Attending,3M 1804 Vflex,12,0,12,1,Yes,Like New,Pass
Male,CRNA,3M 1804 Vflex,12,2,30,2.5,Yes,Good,Pass
Female,Fellow,3m 1860 Teal,13,0,26,2,Yes,Good,Fail
Female,CRNA,3m 1860 Teal,13,1,35,2.69230769230769,Yes,Poor,Fail
Female,Resident,3M 1804 Vflex,14,1,20,1.42857142857143,Yes,Like New,Pass
Male,Resident,3M 1804 Vflex,14,2,15,1.07142857142857,Yes,Like New,Pass
Female,CRNA,3m 1860 Teal,14,1,50,3.57142857142857,Yes,Good,Pass
Female,CRNA,3M 1804 Vflex,15,3,30,2,No,Good,Fail
Female,CRNA,3M 1804 Vflex,15,1,30,2,Yes,Good,Fail
Female,CRNA,3m 1860 Teal,15,1,40,2.66666666666667,Yes,Good,Fail
Male,Resident,3M 1804 Vflex,15,1,30,2,Yes,Good,Pass
Female,CRNA,3m 1860 Teal,15,1,50,3.33333333333333,Yes,Like New,Pass
Female,CRNA,3M 1804 Vflex,20,3,35,1.75,Yes,Poor,Fail
Female,Resident,3M 1804 Vflex,20,4,35,1.75,No,Good,Fail
Female,CRNA,3M 1804 Vflex,20,2,40,2,No,Good,Pass
Female,CRNA,3M 1804 Vflex,30,6,180,6,No,Good,Fail
Female,CRNA,3m 1860 Teal,40,2,60,1.5,Yes,Like New,Fail
Male,CRNA,3m 1860 Teal,60,4,180,3,Yes,Good,Pass
    branches: [ "main" ]
  pull_request: 
@article{vandormael_estimating_2019,
        title = {Estimating trends in the incidence rate with interval censored data and time-dependent covariates:},
        url = {https://journals.sagepub.com/doi/full/10.1177/0962280219829892},
        doi = {10.1177/0962280219829892},
        shorttitle = {Estimating trends in the incidence rate with interval censored data and time-dependent covariates},
        abstract = {We propose a multiple imputation method for estimating the incidence rate with interval censored data and time-dependent (and/or time-independent) covariates. T...},
        journaltitle = {Statistical Methods in Medical Research},
        author = {Vandormael, Alain and Tanser, Frank and Cuadros, Diego and Dobra, Adrian},
        urldate = {2020-05-25},
        date = {2019-02-19},
        langid = {english},
        note = {Publisher: {SAGE} {PublicationsSage} {UK}: London, England},
        file = {Snapshot:/home/ryan/Zotero/storage/YU742AJ5/0962280219829892.html:text/html}
}

@article{heller_proportional_2011,
        title = {Proportional hazards regression with interval censored data using an inverse probability weight},
        volume = {17},
        issn = {1380-7870},
        url = {https://www.ncbi.nlm.nih.gov/pmc/articles/PMC5499516/},
        doi = {10.1007/s10985-010-9191-8},
        abstract = {The prevalence of interval censored data is increasing in medical studies due to the growing use of biomarkers to define a disease progression endpoint. Interval censoring results from periodic monitoring of the progression status. For example, disease progression is established in the interval between the clinic visit where progression is recorded and the prior clinic visit where there was no evidence of disease progression. A methodology is proposed for estimation and inference on the regression coefficients in the Cox proportional hazards model with interval censored data. The methodology is based on estimating equations and uses an inverse probability weight to select event time pairs where the ordering is unambiguous. Simulations are performed to examine the finite sample properties of the estimate and a colon cancer data set is used to demonstrate its performance relative to the conventional partial likelihood estimate that ignores the interval censoring.},
        pages = {373--385},
        number = {3},
        journaltitle = {Lifetime data analysis},
        shortjournal = {Lifetime Data Anal},
        author = {Heller, Glenn},
        urldate = {2020-05-25},
        date = {2011-07},
        pmid = {21191653},
        pmcid = {PMC5499516},
        file = {PubMed Central Full Text PDF:/home/ryan/Zotero/storage/GA62YYSW/Heller - 2011 - Proportional hazards regression with interval cens.pdf:application/pdf}
}

@article{wang_flexible_2016,
        title = {A Flexible, Computationally Efficient Method for Fitting the Proportional Hazards Model to Interval-Censored Data},
        volume = {72},
        issn = {0006-341X},
        url = {https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4803641/},
        doi = {10.1111/biom.12389},
        abstract = {The proportional hazards model ({PH}) is currently the most popular regression model for analyzing time-to-event data. Despite its popularity, the analysis of interval-censored data under the {PH} model can be challenging using many available techniques. This paper presents a new method for analyzing interval-censored data under the {PH} model. The proposed approach uses a monotone spline representation to approximate the unknown nondecreasing cumulative baseline hazard function. Formulating the {PH} model in this fashion results in a finite number of parameters to estimate while maintaining substantial modeling flexibility. A novel expectation-maximization ({EM}) algorithm is developed for finding the maximum likelihood estimates of the parameters. The derivation of the {EM} algorithm relies on a two-stage data augmentation involving latent Poisson random variables. The resulting algorithm is easy to implement, robust to initialization, enjoys quick convergence, and provides closed-form variance estimates. The performance of the proposed regression methodology is evaluated through a simulation study, and is further illustrated using data from a large population-based randomized trial designed and sponsored by the United States National Cancer Institute.},
        pages = {222--231},
        number = {1},
        journaltitle = {Biometrics},
        shortjournal = {Biometrics},
        author = {Wang, Lianming and {McMahan}, Christopher S. and Hudgens, Michael G. and Qureshi, Zaina P.},
        urldate = {2020-05-25},
        date = {2016-03},
        pmid = {26393917},
        pmcid = {PMC4803641},
        file = {PubMed Central Full Text PDF:/home/ryan/Zotero/storage/Y76LRP5W/Wang et al. - 2016 - A Flexible, Computationally Efficient Method for F.pdf:application/pdf}
}

@article{bouaziz_regression_2018,
        title = {Regression modelling of interval censored data based on the adaptive ridge procedure},
        url = {http://arxiv.org/abs/1812.09158},
        abstract = {We consider the Cox model with piecewise constant baseline hazard to deal with a mixed case of left-censored, interval-censored and right-censored data. Estimation is carried out with the {EM} algorithm by treating the true event times as unobserved variables. This estimation procedure is shown to produce a block diagonal Hessian matrix of the baseline parameters. Taking advantage of this interesting feature of the estimation procedure a L0 penalised likelihood method is implemented in order to automatically determine the number and locations of the cuts of the baseline hazard. The method is directly extended to the inclusion of exact observations and to a cure fraction. Statistical inference of the model parameters is derived from likelihood theory. Through simulation studies, the penalisation technique is shown to provide a good ﬁt of the baseline hazard and precise estimations of the resulting regression parameters. The method is illustrated on a dental dataset where the eﬀect of covariates on the risk of ankylosis for replanted teeth is assessed.},
        journaltitle = {{arXiv}:1812.09158 [stat]},
        author = {Bouaziz, Olivier and Lauridsen, Eva and Nuel, Grégory},
        urldate = {2020-05-25},
        date = {2018-12-21},
        langid = {english},
        eprinttype = {arxiv},
        eprint = {1812.09158},
        keywords = {Statistics - Methodology},
        file = {Bouaziz et al. - 2018 - Regression modelling of interval censored data bas.pdf:/home/ryan/Zotero/storage/TBRSNCJG/Bouaziz et al. - 2018 - Regression modelling of interval censored data bas.pdf:application/pdf}
}

@article{zhao_simultaneous_2019,
        title = {Simultaneous Estimation and Variable Selection for Interval-Censored Data With Broken Adaptive Ridge Regression},
        rights = {© 2019 American Statistical Association},
        issn = {10.1080/01621459.2018.1537922},
        url = {https://amstat.tandfonline.com/doi/abs/10.1080/01621459.2018.1537922},
        abstract = {(2020). Simultaneous Estimation and Variable Selection for Interval-Censored Data With Broken Adaptive Ridge Regression. Journal of the American Statistical Association: Vol. 115, No. 529, pp. 204-216.},
        journaltitle = {Journal of the American Statistical Association},
        author = {Zhao, Hui and Wu, Qiwei and Li, Gang and Sun, Jianguo},
        urldate = {2020-05-25},
        date = {2019-04-22},
        langid = {english},
        note = {Publisher: Taylor \& Francis}
}

@article{li_adaptive_2019,
        title = {Adaptive lasso for the Cox regression with interval censored and possibly left truncated data:},
        url = {https://journals.sagepub.com/doi/full/10.1177/0962280219856238},
        doi = {10.1177/0962280219856238},
        shorttitle = {Adaptive lasso for the Cox regression with interval censored and possibly left truncated data},
        abstract = {We propose a penalized variable selection method for the Cox proportional hazards model with interval censored data. It conducts a penalized nonparametric maxim...},
        journaltitle = {Statistical Methods in Medical Research},
        author = {Li, Chenxi and Pak, Daewoo and Todem, David},
        urldate = {2020-05-25},
        date = {2019-06-16},
        langid = {english},
        note = {Publisher: {SAGE} {PublicationsSage} {UK}: London, England}
}

@book{groeneboom_information_1992,
        title = {Information Bounds and Nonparametric Maximum Likelihood Estimation},
        isbn = {978-3-7643-2794-1},
        url = {https://www.springer.com/gp/book/9783764327941},
        series = {Oberwolfach Seminars},
        abstract = {This book contains the lecture notes for a {DMV} course presented by the authors at Gunzburg, Germany, in September, 1990. In the course we sketched the theory of information bounds for non parametric and semiparametric models, and developed the theory of non parametric maximum likelihood estimation in several particular inverse problems: interval censoring and deconvolution models. Part I, based on Jon Wellner's lectures, gives a brief sketch of information lower bound theory: Hajek's convolution theorem and extensions, useful minimax bounds for parametric problems due to Ibragimov and Has'minskii, and a recent result characterizing differentiable functionals due to van der Vaart (1991). The differentiability theorem is illustrated with the examples of interval censoring and deconvolution (which are pursued from the estimation perspective in part {II}). The differentiability theorem gives a way of clearly distinguishing situations in which 1 2 the parameter of interest can be estimated at rate n / and situations in which this is not the case. However it says nothing about which rates to expect when the functional is not differentiable. Even the casual reader will notice that several models are introduced, but not pursued in any detail; many problems remain. Part {II}, based on Piet Groeneboom's lectures, focuses on non parametric maximum likelihood estimates ({NPMLE}'s) for certain inverse problems. The first chapter deals with the interval censoring problem.},
        publisher = {Birkhäuser Basel},
        author = {Groeneboom, P. and Wellner, J. A.},
        urldate = {2020-05-26},
        date = {1992},
        langid = {english},
        doi = {10.1007/978-3-0348-8621-5},
        file = {Snapshot:/home/ryan/Zotero/storage/69R843P3/9783764327941.html:text/html}
}

@article{lindsey_study_1998,
        title = {A study of interval censoring in parametric regression models},
        volume = {4},
        issn = {1380-7870},
        doi = {10.1023/a:1009681919084},
        abstract = {Parametric models for interval censored data can now easily be fitted with minimal programming in certain standard statistical software packages. Regression equations can be introduced, both for the location and for the dispersion parameters. Finite mixture models can also be fitted, with a point mass on right (or left) censored observations, to allow for individuals who cannot have the event (or already have it). This mixing probability can also be allowed to follow a regression equation. Here, models based on nine different distributions are compared for three examples of heavily censored data as well as a set of simulated data. We find that, for parametric models, interval censoring can often be ignored and that the density, at centres of intervals, can be used instead in the likelihood function, although the approximation is not always reliable. In the context of heavily interval censored data, the conclusions from parametric models are remarkably robust with changing distributional assumptions and generally more informative than the corresponding non-parametric models.},
        pages = {329--354},
        number = {4},
        journaltitle = {Lifetime Data Analysis},
        shortjournal = {Lifetime Data Anal},
        author = {Lindsey, J. K.},
        date = {1998},
        pmid = {9880994},
        keywords = {Animals, Biometry, Breast Neoplasms, Data Interpretation, Statistical, Disease-Free Survival, Female, Fishes, {HIV} Infections, Humans, Incidence, Models, Statistical, Normal Distribution, Regression Analysis, Statistics, Nonparametric, Survival Rate, Zinc}
}

@article{liao_cgam_2019,
        title = {cgam: An R Package for the Constrained Generalized Additive Model},
        volume = {89},
        rights = {Copyright (c) 2019 Xiyue Liao, Mary C. Meyer},
        issn = {1548-7660},
        url = {https://www.jstatsoft.org/index.php/jss/article/view/v089i05},
        doi = {10.18637/jss.v089.i05},
        shorttitle = {cgam},
        pages = {1--24},
        number = {1},
        journaltitle = {Journal of Statistical Software},
        
        urldate = {2020-05-26},
        date = {2019-05-10},
        langid = {english},
        note = {Number: 1},
        keywords = {constrained generalized additive model, graphical routine, isotonic regression, iteratively re-weighted cone projection, partial linear, R, spline regression},
        file = {Full Text PDF:/home/ryan/Zotero/storage/DUNV9JW5/Liao and Meyer - 2019 - cgam An R Package for the Constrained Generalized.pdf:application/pdf;Snapshot:/home/ryan/Zotero/storage/M8M2A7XS/v089i05.html:text/html}
}

@article{jackson_flexsurv_2016,
        title = {flexsurv: A Platform for Parametric Survival Modeling in R},
        volume = {70},
        rights = {Copyright (c) 2016 Christopher Jackson},
        issn = {1548-7660},
        url = {https://www.jstatsoft.org/index.php/jss/article/view/v070i08},
        doi = {10.18637/jss.v070.i08},
        shorttitle = {flexsurv},
        pages = {1--33},
        number = {1},
        journaltitle = {Journal of Statistical Software},
        author = {Jackson, Christopher},
        urldate = {2020-05-26},
        date = {2016-05-12},
        langid = {english},
        note = {Number: 1},
        keywords = {multi-state models, multistate models, survival},
        file = {Full Text:/home/ryan/Zotero/storage/H4NEZ4GC/Jackson - 2016 - flexsurv A Platform for Parametric Survival Model.pdf:application/pdf;Snapshot:/home/ryan/Zotero/storage/LC9NLU55/v070i08.html:text/html}
}

@article{anderson-bergman_icenreg_2017,
        title = {{icenReg}: Regression Models for Interval Censored Data in R},
        volume = {81},
        rights = {Copyright (c) 2017 Clifford Anderson-Bergman},
        issn = {1548-7660},
        url = {https://www.jstatsoft.org/index.php/jss/article/view/v081i12},
        doi = {10.18637/jss.v081.i12},
        shorttitle = {{icenReg}},
        pages = {1--23},
        number = {1},
        journaltitle = {Journal of Statistical Software},
        author = {Anderson-Bergman, Clifford},
        urldate = {2020-05-26},
        date = {2017-11-13},
        langid = {english},
        note = {Number: 1},
        keywords = {accelerated failure time, interval censoring, non-parametric, proportional hazards, proportional odds, semi-parametric regression, survival analysis},
        file = {Full Text:/home/ryan/Zotero/storage/88DZJ3VM/Anderson-Bergman - 2017 - icenReg Regression Models for Interval Censored D.pdf:application/pdf;Snapshot:/home/ryan/Zotero/storage/9PCIZ4VD/v081i12.html:text/html}
}
    branches: [ "main" ]
jobs: library(magrittr)
library(tidyverse)
library(survival)
library("interval")
library(cgam)
library(icenReg)
library("km.ci")
library(flexsurv)
library(tableone)
set.seed(101)

missing_packages <- setdiff(c("caret", "tableone", "rmarkdown", "knitr", "bookdown") , rownames(installed.packages() ) ) 

if(length(missing_packages) > 0) {
stop(paste("install" , paste0(missing_packages, collapse =" , ") ))
}
##############
#### data load and procesing
##############
prelim.df <- read_csv(file="Final Pilot Data.csv")

## this variable controls the minimum amount of time a mask is assumed to function; the analysis assumes ALL masks functioned before this time since all users had passed fit tests and masks undergo factory qc. it is expressed in days
immortal_time <- 0.5

## drop the redundant column
names(prelim.df) %<>% make.names
prelim.df %<>% rename(fit.fail = Qualitative.Fit)
prelim.df %<>% mutate(fit.fail = fit.fail == "Fail")
prelim.df %<>% mutate_if( .predicate=is.character, factor)
prelim.df %<>% mutate_at(vars(one_of("fit.fail","Days.Worn", "Sterilizations", "Uses")), as.integer  )
prelim.df %<>% mutate(left=if_else(fit.fail==1, immortal_time, as.numeric(Days.Worn)  ) , right=if_else(fit.fail==1, as.numeric(Days.Worn), as.numeric(max(Days.Worn )*3 )  )   )

##############
### cross-sectional analysis
##############

## this is my preferred estimate
cross_fit <- cgam(fit.fail ~ s.incr(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = 4:20)
dummy_data <- cbind(dummy_data, predict(cross_fit, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )

dummy_data %<>% filter(Days.Worn %in% c(5,10,15) )
dummy_data



## supplement plot comparing methods
## black = non-parametric monotone decreasing (no smoothness constrain), identical to the nonparametric survival estimator
## red = smooth monotone decreasing (spline basis, constrained)
## blue = smooth no monotone requirement (spline bases)
## conclusion: extremely similar, observed fit failure pretty flat across time
cross_fit <- cgam(fit.fail ~ incr(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = 4:20)
dummy_data <- cbind(dummy_data, predict(cross_fit, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
plot(dummy_data$Days.Worn, dummy_data$fit , xlab="Days Used", ylab="Failure probability", ylim=c(0,1), xlim=c(2,20), type="b", pch=19)
lines(dummy_data$Days.Worn, dummy_data$lower, lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, lty=2)

cross_fit <- cgam(fit.fail ~ s.incr(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = 4:20)
dummy_data <- cbind(dummy_data, predict(cross_fit, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
points(dummy_data$Days.Worn, dummy_data$fit , col='red', type="b", pch=19)
lines(dummy_data$Days.Worn, dummy_data$lower, col='red', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='red', lty=2)


cross_fit <- glm(fit.fail ~ ns(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = 4:20)
dummy_data <- cbind(dummy_data, predict(cross_fit, newdata=dummy_data, se.fit=TRUE)[c("fit", "se.fit")] %>% as.data.frame )

points(dummy_data$Days.Worn, cross_fit$family$linkinv(dummy_data$fit) , col='blue', type="b", pch=19)
dummy_data %<>% mutate( lower=cross_fit$family$linkinv(fit - 1.96*se.fit), upper=cross_fit$family$linkinv(fit+1.96*se.fit))
lines(dummy_data$Days.Worn, dummy_data$lower, col='blue', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='blue', lty=2)




## supplement plot comparing methods
## black = nonparametric survival estimator with bootstrap (interval package)
## red = parametric survival (generalized gamma)
## blue = monotone smooth spline
## conclusion: survival model forces the early survival upwards
npm_out <- icfit( Surv( left, right, type = "interval2") ~ 1, data = prelim.df, conf.int=TRUE, control = icfitControl(B=2000,seed=1234))
fs_1 <- flexsurvreg(Surv(left, right, type="interval2")~1, data=prelim.df, dist="gengamma")
cross_fit <- cgam(I(1L-fit.fail) ~ s.decr(Days.Worn) , family=binomial(), data=prelim.df )
# fs_2 <- flexsurvspline(Surv(left, right, type="interval2")~1, data=prelim.df, timescale="log", k=1L) ## opaque error



npm_out %>% plot(xlim=c(2,20))
lines(fs_1)
dummy_data <- data.frame(Days.Worn = 2:20)
dummy_data <- cbind(dummy_data, predict(cross_fit, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
lines(dummy_data$Days.Worn, dummy_data$fit , col='blue', lwd=2)
lines(dummy_data$Days.Worn, dummy_data$lower, col='blue', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='blue', lty=2)

##ICsurv method, is similar
if(FALSE) {
exp_res<-ICsurv::fast.PH.ICsurv.EM(d1=rep(0L, nrow(prelim.df)), d3=rep(0L, nrow(prelim.df)), d2=rep(1L, nrow(prelim.df)), Li=prelim.df$left, Ri=prelim.df$right, Xp=matrix(runif(nrow(prelim.df)), ncol=1) , n.int=3, order=3, g0=rep(1, 3+3) , b0=rep(0,1L), tol=0.001, equal=TRUE, t.seq=4:20)

points(dummy_data$Days.Worn, 1.-exp_res$hz, col="green", type="b", pch=19)
}

## same calculation, different package to check
if(FALSE) {
alt_np <- ic_np(cbind(left, right)~0, data=prelim.df )
getSCurves(alt_np )
}



## supplement plot: comparison of parametric survival families
## conclusion : very similar , generalized gamma probably the best fitting

## to show added uncertainty from the distribution choice
npm_out %>% plot(xlim=c(2,20))
lines(fs_1, type = "survival", col="red")

fs2 <- flexsurvreg(Surv(left, right, type="interval2")~1, data = prelim.df, dist="gompertz")
lines(fs2, type = "survival", col="blue")

fs2 <- flexsurvreg(Surv(left, right, type="interval2")~1, data = prelim.df, dist="weibull")
lines(fs2, type = "survival", col="green")



## supplement plot: comparison of interval censored and "traditional" survival analysis
## conclusion: traditional survival badly biased (as expected)
npm_out %>% plot(xlim=c(2,20))
lines(km.ci(survfit(Surv(time=Days.Worn, event=fit.fail)~1, data=prelim.df) ), col='red')




## analytical claims:
## 1: # steralizations doesn't matter conditional on number of days
## survival semi-para: 0.17 +- 0.27 log scale
## cross-sectional semi-para: .34 +- .34
np_reg <- ic_sp(cbind(left, right)~Sterilizations, data=prelim.df, bs_samples = 1000  )
np_reg %>% summary

cross_fit <- glm(fit.fail ~ ns(Days.Worn) , family=binomial(), data=prelim.df )
cross_fit2 <- glm(fit.fail ~ ns(Days.Worn) +Sterilizations  , family=binomial(), data=prelim.df )
anova(cross_fit2, cross_fit, test="LRT")
cross_fit2 %>% summary
glm(fit.fail ~ Sterilizations , family=binomial(), data=prelim.df ) %>% summary


## 2: number of times donned doesn't matter
##  survival semi-para: -0.005 +- 0.23 log scale
##  cross sectional: -0.0007505  0.0243780
np_reg2a <- ic_sp(cbind(left, right)~Uses, data=prelim.df, bs_samples = 1000  )
np_reg2a %>% summary

cross_fit <- glm(fit.fail ~ ns(Days.Worn) , family=binomial(), data=prelim.df )
cross_fit2 <- glm(fit.fail ~ ns(Days.Worn) +Uses  , family=binomial(), data=prelim.df )
anova(cross_fit2, cross_fit, test="LRT")
cross_fit2 %>% summary
glm(fit.fail ~ Uses , family=binomial(), data=prelim.df ) %>% summary

## 2b per day
## 0.01764 0.1975
## -0.05921    0.22283
np_reg2b <- ic_sp(cbind(left, right)~I(Uses/Days.Worn), data=prelim.df, bs_samples = 1000  )
np_reg2b %>% summary

cross_fit <- glm(fit.fail ~ ns(Days.Worn) , family=binomial(), data=prelim.df )
cross_fit2 <- glm(fit.fail ~ ns(Days.Worn) +I(Uses/Days.Worn)  , family=binomial(), data=prelim.df )
anova(cross_fit2, cross_fit, test="LRT")
cross_fit2 %>% summary
glm(fit.fail ~ I(Uses/Days.Worn) , family=binomial(), data=prelim.df ) %>% summary

## 3: assessment of fit probably does matter
##  -1.071  2.412 decent sized effect!
##  -1.8021     0.8696 p 0.02249 by lrt
##  about the same in marginal model -1.8137     0.8574    0.0344 * (p=.033 by fet)

np_reg3 <- ic_sp(cbind(left, right)~Fits.well, data=prelim.df, bs_samples = 1000  )
np_reg3 %>% summary

cross_fit <- glm(fit.fail ~ ns(Days.Worn) , family=binomial(), data=prelim.df )
cross_fit2 <- glm(fit.fail ~ ns(Days.Worn) +Fits.well  , family=binomial(), data=prelim.df )
anova(cross_fit2, cross_fit, test="LRT")
cross_fit2 %>% summary
glm(fit.fail ~ Fits.well , family=binomial(), data=prelim.df ) %>% summary
prelim.df %>% group_by(Fits.well) %>% summarize(mean(fit.fail ), n())
prelim.df %>% group_by(fit.fail) %>% summarize(mean(as.integer(Fits.well )-1), n())
fisher.test(prelim.df$Fits.well, prelim.df$fit.fail)

## also available as a test: the method matters, the more conservative one does not reject, the more analytical one is marginal
## p=.052, .145
np_test3 <- ictest(Surv( left, right, type = "interval2") ~ Fits.well, data = prelim.df)
np_test3 <- ictest(Surv( left, right, type = "interval2") ~ Fits.well, data = prelim.df, method="wsr.mc")


## plot the difference
## no obvious change in time, both are pretty flat. it's just an offset
# alt_np3 <- icfit( Surv( left, right, type = "interval2") ~ Fits.well, data = prelim.df, conf.int=TRUE, control = icfitControl(B=2000,seed=1234))
# alt_np3 %>% plot(xlim=c(2,20)) # this plot is hard to control

cross_fit2 <- cgam(I(1L-fit.fail) ~ s.decr(Days.Worn)*Fits.well  , family=binomial(), data=prelim.df )

npm_out %>% plot(xlim=c(2,20))

dummy_data <- data.frame(Days.Worn = 4:20 , Fits.well =prelim.df$Fits.well %>% unique %>% magrittr::extract(1) )
dummy_data <- cbind(dummy_data, predict(cross_fit2, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
lines(dummy_data$Days.Worn, dummy_data$fit , col='red', lwd=2)
lines(dummy_data$Days.Worn, dummy_data$lower, col='red', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='red', lty=2)

dummy_data <- data.frame(Days.Worn = 4:20 , Fits.well =prelim.df$Fits.well %>% unique %>% magrittr::extract(2) )
dummy_data <- cbind(dummy_data, predict(cross_fit2, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
lines(dummy_data$Days.Worn, dummy_data$fit , col='blue', lwd=2)
lines(dummy_data$Days.Worn, dummy_data$lower, col='blue', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='blue', lty=2)


# dummy_data <- cbind(dummy_data, predict(cross_fit2, newdata=dummy_data, se.fit=TRUE)[c("fit", "se.fit")] %>% as.data.frame )
# lines(dummy_data$Days.Worn, cross_fit2$family$linkinv(dummy_data$fit) , col='blue')
# dummy_data %<>% mutate( lower=cross_fit$family$linkinv(fit - 1.96*se.fit), upper=cross_fit$family$linkinv(fit+1.96*se.fit))
# lines(dummy_data$Days.Worn, dummy_data$lower, col='blue', lty=2)
# lines(dummy_data$Days.Worn, dummy_data$upper, col='blue', lty=2)

# dummy_data <- data.frame(Days.Worn = 4:20 , Fits.well =prelim.df$Fits.well %>% unique %>% magrittr::extract(2) )
# dummy_data <- cbind(dummy_data, predict(cross_fit2, newdata=dummy_data, se.fit=TRUE)[c("fit", "se.fit")] %>% as.data.frame )
# lines(dummy_data$Days.Worn, cross_fit2$family$linkinv(dummy_data$fit) , col='red')
# dummy_data %<>% mutate( lower=cross_fit$family$linkinv(fit - 1.96*se.fit), upper=cross_fit$family$linkinv(fit+1.96*se.fit))
# lines(dummy_data$Days.Worn, dummy_data$lower, col='red', lty=2)
# lines(dummy_data$Days.Worn, dummy_data$upper, col='red', lty=2)
# 


#############
## plots by # donned
## supplemental: comparison of methods
prelim.df2 <-prelim.df %>% mutate(left=if_else(fit.fail==1, 1, as.numeric(Uses)  ) , right=if_else(fit.fail==1, as.numeric(Uses), as.numeric(max(Uses )*3 )  )   )

npm_out2 <- icfit( Surv( left, right, type = "interval2") ~ 1, data = prelim.df2, conf.int=TRUE, control = icfitControl(B=2000,seed=1234))

fs3 <- flexsurvreg(Surv(left, right, type="interval2")~1, dist="gengamma", data=prelim.df2)
npm_out2 %>% plot( xlim=c(2,50) )
lines(fs3, col="blue")

## for purposes of this plot, uses are only drawn out to 50. the right outlier makes the spline fit unstable
cross_fit3 <- cgam(I(1L-fit.fail) ~ s.decr(Uses) , family=binomial(), data=prelim.df %>% mutate(Uses = pmin(Uses, 70)) )
dummy_data <- data.frame(Uses = 6:50)
dummy_data <- cbind(dummy_data, predict(cross_fit3, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
lines(dummy_data$Uses, dummy_data$fit , col='red', lwd=2)
lines(dummy_data$Uses, dummy_data$lower, col='red', lty=2)
lines(dummy_data$Uses, dummy_data$upper, col='red', lty=2)


save(file="n95_intermediates.Rdata", prelim.df, prelim.df2, npm_out2, npm_out, np_test3, np_reg3, np_reg2b, np_reg2a, np_reg)




prelim.df %>% group_by(fit.fail) %>% summarize(sum(as.integer(Fits.well )-1), n())

temp <- prelim.df %>% filter(fit.fail==1) %>% summarize(sum(as.integer(Fits.well )-1), n()) %>% unlist
prop.test(x=temp[1], n=temp[2]) %>% extract2("conf.int") %>% multiply_by(100) %>% round


temp <- prelim.df %>% filter(fit.fail==1) %>% mutate(Mask.quality=Mask.quality %in% c("Like New", "Good") ) %>% summarize(sum(as.integer(Mask.quality )), n()) %>% unlist
prop.test(x=temp[1], n=temp[2]) %>% extract2("conf.int") %>% multiply_by(100) %>% round


# PropCIs::scoreci(x=temp[1], n=temp[2], conf.level=0.95) %>% extract2("conf.int") %>% multiply_by(100) %>% round

with(prelim.df, cor(Uses, Days.Worn, method="spearman") )
with(prelim.df, summary(lm(Uses~ Days.Worn) ))


prelim.df %>% select(fit.fail, Fits.well ) %>% table
temp_conf <- with(prelim.df, caret::confusionMatrix(reference=factor(!fit.fail, labels=c("Yes", "No")), data=Fits.well %>% fct_recode(Yes="No", No="Yes" )))

prelim.df %>% mutate(Mask.quality=Mask.quality %in% c("Poor") ) %>% select(Mask.quality, fit.fail ) %>% table

with(prelim.df  %>% mutate(Mask.quality=factor(!(Mask.quality %in% c("Poor")) , labels=c("Yes", "No" ) )), caret::confusionMatrix(reference=factor(fit.fail, labels=c("Yes", "No")), data=Mask.quality))

# temp_t1 <- table1(~Sex + Title + respirator + Days.Worn + Sterilizations + Uses + I(Uses/Days.Worn ) +Fits.well + Mask.quality | factor(fit.fail, labels=c("Yes", "No"))  , data=prelim.df)

factor_vars<- c('Sex' , 'Title' , 'Respirator', 'Fits.well', 'Mask.quality'  )

con_vars<- c( 'Days.Worn', 'Sterilizations', 'Uses',  'Uses.per.day' )
non_vars <- c( 'Days.Worn', 'Sterilizations', 'Uses',  'Uses.per.day' )
tab3 <- CreateTableOne(vars = c(con_vars,factor_vars) , strata = "fit.fail" , data = prelim.df%>%  mutate( Uses.per.day=Uses/Days.Worn ) %>% mutate(fit.fail=factor(fit.fail, labels=c("Pass", "Fail")) ) , factorVars = factor_vars )
temp <- capture.output(x <- print(tab3 , showAllLevels = TRUE, contDigits=1, printToggle=FALSE, nonnormal = non_vars))

temp <- print(tab3, showAllLevels = TRUE, contDigits=1)

pdf(file="failure_prob.pdf")
par( mar=c(4.1, 4.1, 1, 3.1) )
cross_fit <- cgam(fit.fail ~ incr(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = 2:20)
dummy_data <- cbind(dummy_data, predict(cross_fit, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )


temp_hist <- hist(prelim.df  %>% select(Days.Worn) %>% unlist ,breaks = seq(from=0.5, to=60.5), plot=T, axes=F, xlab=NULL, ylab=NULL, xlim=c(1.5,20) , main=NULL, col= rgb(0.6,1.0,1.0,alpha=0.5), border=rgb(0.6,1.0,1.0,alpha=0.5), freq=TRUE)

axis(4)

par(new=T)

plot(dummy_data$Days.Worn, dummy_data$fit*100 , xlab="Days worn", ylab="Failure probability (%)", ylim=c(0,100), xlim=c(1.5,20), type="l", col="black", lwd=2)
lines(dummy_data$Days.Worn, dummy_data$lower*100, lty=2, col="black", lwd=2)
lines(dummy_data$Days.Worn, dummy_data$upper*100, lty=2, col="black", lwd=2)


mtext(side=4, text="Count of Days Used", line=2)
par(new=F)


dev.off()

prelim.df %>% mutate(broken_days = cut(Days.Worn, breaks=c(1, 5, 8, 12, 25)) ) %>% select(broken_days) %>% table

# prelim.df %>% mutate(broken_days = cut(Days.Worn, breaks=c(1, 5, 8, 12, 25)) ) %>% group_by(broken_days) %>% summarize(n=n() , mean.failure=mean(fit.fail )%>% round(2), lcb=prop.test(x=sum(fit.fail), n=n())$conf.int[1]%>% round(2), ucb=prop.test(x=sum(fit.fail), n=n())$conf.int[2]%>% round(2) ) %>% {knitr::kable(x=.,format="html")}



with(prelim.df , scatter.smooth(Days.Worn, fit.fail ))


  test: ---
output: 
  bookdown::pdf_document2:
    fig_caption: true
    toc: false
    keep_tex: yes
title: "Web Appendix for Probability of Fit Failure with Reuse of N95 Respirators"
author: 
 -"

date: "`r format(Sys.time(), '%B %d, %Y')`"
bibliography: n95.bib

---

```{r echo=FALSE, message=FALSE, results='hide'}
library(magrittr)
if(file.exists("n95_intermediates.Rdata"))  {
load("n95_intermediates.Rdata")
library("magrittr")
library("tidyverse")
library("survival")
library("rmarkdown")
library("interval")
library("cgam")
library("icenReg")
library("km.ci")
library("flexsurv")
library("bookdown")
library("tableone")
library("PropCIs")
} else {
source("n95_report.R")
}
```

```{r setup, include=FALSE}
knitr::opts_chunk$set(dev = 'pdf')
```



# Modeling assumptions and strategy
The sample is generated cross-sectionally, but can also be thought of as survival data.
Each fit test provides interval censored information: masks observed to fail a fit test at time *t* failed at some point in $\left(0 , t\right)$ (masks are assumed to have been initially functional). 
Masks that pass the fit test fail some time in the future $(t, t_\textrm{max})$.
For survival analysis we assume that each mask is independent with failure hazard from from a common distribution.
Our parametric survival model fits a 3-parameter generalized gamma hazard to the interval censored data.

Non-parametric or semi-parametric estimators which do not assume a functional form of the hazard are common in traditional survival analysis, and are available for interval censored data as well.
However, non-parametric estimates (NPMLE) for interval censored data are not unique; in some regions any baseline hazard matching continuity requirements is equally supported by the data.
Confidence intervals for interval censored baseline hazards are an active area of research [@wang_flexible_2016; @zhao_simultaneous_2019], with completely non-parametric estimation methods yielding slow $n^\frac{1}{3}$ convergence without a known asymptotic law [@groeneboom_information_1992].
Several semi-parametric methods have recently been shown to have $n^\frac{1}{2}$ convergence with Gaussian asymptotics making them amenable to bootstrapping [@wang_flexible_2016; @bouaziz_regression_2018; @zhao_simultaneous_2019; @li_adaptive_2019].
Proportional hazards regression was used to model factors other than duration of use in this framework.

Modeling the data as cross-sectional observations of fit passing versus failure as a function of time is also supportable because no participant-mask pair is observed more than once and there is minimal information to support extrapolation from before the earliest test time.
Because only a very small number of individuals are tested on more than one mask, we do not model or adjust for participant effects.
Integrated b-splines and constrained step functions allow modeling the failure log-odds as a monotone increasing function of time in a traditional logistic regression analysis.
This approach yields much better studied statistical behavior; however, given the modest sample size the probability statements produced should be taken as approximate.
The cross-sectional model does not encode any behavior before the earliest test time (e.g. that all masks initially fit).
We found that the cross-sectional model and non-parametric and semi-parametric survival models yield essentially identical point estimates when tuned appropriately, which is anticipated based on the use of step functions and splines to model the baseline hazard in the survival routines.

Factors beyond use duration were analyzed with proportional hazards regression (interval survival) or logistic regression (cross sectional).
Calculations were performed in R 3.6.3 with package flexsurv [@jackson_flexsurv_2016] for parametric survival modeling, icenReg [@anderson-bergman_icenreg_2017] for non-parametric survival modeling, and cgam [@liao_cgam_2019] for monotone logistic regression. 
Reported confidence intervals are at the 0.95 level.
Because this was an exploratory study, no pre-determined p-value threshold was applied.
Code and data to recreate this report can be found at https://github.com/cryanking/n95_refit

# Results
```{r echo=FALSE}
cross_fit <- cgam(fit.fail ~ s.incr(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = c(4,10,15))
dummy_data <- cbind(dummy_data, predict(cross_fit, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
```

```{r echo=FALSE, messages=FALSE, results='asis'}
factor_vars<- c('Sex' , 'Title' , 'Respirator', 'Fits.well', 'Mask.quality'  )
non_vars <- c( 'Days.Worn', 'Sterilizations', 'Uses' )
con_vars<- c( 'Days.Worn', 'Sterilizations', 'Uses' )
tab3 <- CreateTableOne(vars = c(con_vars,factor_vars) , strata = "fit.fail" , data = prelim.df%>%  mutate(fit.fail=factor(fit.fail, labels=c("Pass", "Fail")) )  , factorVars = factor_vars)
temp <- capture.output(x <- print(tab3 , showAllLevels = TRUE, contDigits=0, printToggle=FALSE, nonnormal=TRUE, exact=T))

tab2 <- CreateTableOne(vars ='Uses.per.day' , strata = "fit.fail" , data = prelim.df%>%  mutate( Uses.per.day=Uses/Days.Worn ) %>% mutate(fit.fail=factor(fit.fail, labels=c("Pass", "Fail")) )  )

temp <- capture.output(x2<- print(tab2 , showAllLevels = TRUE, contDigits=1, printToggle=FALSE, nonnormal=FALSE, exact=T))

x<- rbind(x[1:length(con_vars), ,drop=FALSE], x2[-1,,drop=FALSE], x[-c(1:length(con_vars)), ,drop=FALSE] )

x <- x[,seq(ncol(x)-1)]
rownames(x) <- gsub(rownames(x), pattern=".", fixed=TRUE, replacement=" " )
    knitr::kable(x, caption="\\label{tab:desc}Descriptive statistics stratified by fit test failure. P-values for quantitative variables by Mann-Whitney U, factor variables by Fisher's exact test.")
```

Table \@ref(tab:desc) provides descriptive statistics.
Notably, male participants were less likely to fail fit tests both marginally and adjusting for the number of days worn (OR = `r temp <- glm(fit.fail ~ ns(Days.Worn) + Sex, data=prelim.df); temp2<- temp %>% coef %>% exp %>% extract2(3) %>% round(1); temp2 <- paste(temp2, "95% CI", paste(temp%>% confint %>% exp %>% magrittr::extract(3,1:2) %>% round(1), collapse=" to ") ); temp2   `).
Figure \@ref(fig:hists) displays histograms of number of days worn, number of times used, and number of times sterilized.
Surprisingly, the use duration and intensity was not markedly different between failed and passed mask fits.
Breaking the data into groups by number of days worn, the failure fraction is displayed in Table \@ref(tab:broken).
Table \@ref(tab:broken) shows that the failure fraction at all times is high; approximately 50% in all time categories.

```{r echo=FALSE, fig.show="hold", fig.keep="last", fig.cap="\\label{fig:hists}Mask use histograms."}
par(mfrow=c(3,1), oma=c(0,0,0,0), mar=c(2, 3, 2,0))
hist(prelim.df  %>% select(Days.Worn) %>% unlist ,breaks = seq(from=0.5, to=60.5), plot=T, main="Days Worn", xlab=NULL, col= rgb(0.6,1.0,1.0,alpha=0.5))
hist(prelim.df  %>% select(Uses) %>% unlist , breaks=20, plot=T, main="Times used", xlab=NULL, col= rgb(0.6,1.0,1.0,alpha=0.5))
hist(prelim.df  %>% select(Sterilizations) %>% unlist ,  plot=T, main="Times sterilized", xlab=NULL, col= rgb(0.6,1.0,1.0,alpha=0.5))
par(mfrow=c(1,1))
```


```{r echo=FALSE, results='asis' }
prelim.df %>% mutate(day_group = cut(Days.Worn, breaks=c(0.5, 5, 8, 12, 61)) ) %>% group_by(day_group) %>% summarize(n=n() , mean.failure=mean(fit.fail )%>% round(2), lower.conf=PropCIs::scoreci(x=sum(fit.fail), n=n(), conf.level=0.95)$conf.int[1]%>% round(2), upper.conf=PropCIs::scoreci(x=sum(fit.fail), n=n(), conf.level=0.95)$conf.int[2]%>% round(2) ) %>% {knitr::kable(x=.,format="latex", caption="\\label{tab:broken}Failure fractions by duration worn. Confidence intervals by Wilson's score method. ")}
```

Figure \@ref(fig:basicsurvival) displays mask survival as a function of days worn using cross-sectional approaches.
A monotone fit (I-splines) and natural spline (no restrictions) can be seen to agree well with non-parametric monotone methods.
The failure fraction at day `r dummy_data[1, "Days.Worn"]%>% as.integer` is `r dummy_data[1, "fit"]%>% round(2)` (95\% CI `r round(dummy_data[1,c("lower", "upper")],2)`). At day `r dummy_data[2, "Days.Worn"]%>% as.integer` the failure has increased to `r dummy_data[2, "fit"]%>% round(2)` (95\% CI `r round(dummy_data[2,c("lower", "upper")],2)`) and day `r dummy_data[3, "Days.Worn"]%>% as.integer` to `r dummy_data[3, "fit"]%>% round(2)` (95\% CI `r round(dummy_data[3,c("lower", "upper")],2)`).


```{r echo=FALSE, fig.show="hold", fig.keep="last", fig.cap="\\label{fig:basicsurvival} Comparison of cross sectional logistic regression models for mask Failure by number of days worn. Black = non-parametric monotone (step functions), Red = semi-parametric smooth with monotone constraint, Blue = semi-parametric smooth (no monotone requirement). Dashed lines 95% point-wise confidence limits. Fit with cgam package."}
cross_fit <- cgam(fit.fail ~ incr(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = 4:20)
dummy_data <- cbind(dummy_data, predict(cross_fit, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
plot(dummy_data$Days.Worn, dummy_data$fit , xlab="Days worn", ylab="Failure probability", ylim=c(0,1), xlim=c(4,20), type="l")
lines(dummy_data$Days.Worn, dummy_data$lower, lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, lty=2)

cross_fit <- cgam(fit.fail ~ s.incr(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = 4:20)
dummy_data <- cbind(dummy_data, predict(cross_fit, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
points(dummy_data$Days.Worn, dummy_data$fit , col='red', type="l")
lines(dummy_data$Days.Worn, dummy_data$lower, col='red', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='red', lty=2)


cross_fit <- glm(fit.fail ~ ns(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = 4:20)
dummy_data <- cbind(dummy_data, predict(cross_fit, newdata=dummy_data, se.fit=TRUE)[c("fit", "se.fit")] %>% as.data.frame )

points(dummy_data$Days.Worn, cross_fit$family$linkinv(dummy_data$fit) , col='blue', type="l")
dummy_data %<>% mutate( lower=cross_fit$family$linkinv(fit - 1.96*se.fit), upper=cross_fit$family$linkinv(fit+1.96*se.fit))
lines(dummy_data$Days.Worn, dummy_data$lower, col='blue', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='blue', lty=2)
```

Figure \@ref(fig:comparesurv) compares interval censored survival-based analyses and the cross-sectional analysis.
The parametric model diverges from the non-parametric MLE at the extremes of time, as it requires a smoother change between the initial state (0 failure) to the early observed times.
The generalized gamma and Weibull parametric families fit well.
Figure \@ref(fig:comparepara) displays the results with gamma and weibull hazards.


```{r echo=FALSE, fig.show="hold", fig.keep="last", fig.cap="\\label{fig:comparesurv}Comparison of survival and cross-sectional models for mask failure by number of days worn. Black = non-parametric survival with interval censoring (grey regions = indeterminate MLE), Red = generalized gamma survival model with interval censoring, Blue = cross sectional monotone smooth. Dashed lines 95% point-wise confidence limits. "}
npm_out %>% plot(xlim=c(4,20), XLAB="Days worn", YLAB="Failure probability", dtype="cdf")
fs_1 <- flexsurvreg(Surv(left, right, type="interval2")~1, data=prelim.df, dist="gengamma")
cross_fit <- cgam(fit.fail ~ s.incr(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = 4:20)
dummy_data <- cbind(dummy_data, predict(cross_fit, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
lines(dummy_data$Days.Worn, dummy_data$fit , col='blue', lwd=2)
lines(dummy_data$Days.Worn, dummy_data$lower, col='blue', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='blue', lty=2)

failure_probs<- summary(fs_1, type="survival", t=4:20, se=TRUE, ci=TRUE)[[1]]

lines(failure_probs$time, 1-failure_probs$est, col="red")
lines(failure_probs$time, 1-failure_probs$lcl, col="red", lty=2)
lines(failure_probs$time, 1-failure_probs$ucl, col="red", lty=2)
```

```{r echo=FALSE, fig.show="hold", warning=FALSE , message=FALSE, fig.keep="last", fig.cap="\\label{fig:comparepara}Comparison of parametric family survival models. Black = non-parametric survival (grey regions = indeterminate MLE), Red = generalized gamma survival, Blue = Weibull. Dashed lines 95% point-wise confidence limits. Fit with cgam package."}
npm_out %>% plot(xlim=c(4,20), XLAB="Days worn", YLAB="Failure probability", dtype="cdf")
lines(failure_probs$time, 1-failure_probs$est, col="red")
lines(failure_probs$time, 1-failure_probs$lcl, col="red", lty=2)
lines(failure_probs$time, 1-failure_probs$ucl, col="red", lty=2)


fs_1 <- flexsurvreg(Surv(left, right, type="interval2")~1, data = prelim.df, dist="weibull")

failure_probs<- summary(fs_1, type="survival", t=4:20, se=TRUE, ci=TRUE)[[1]]
lines(failure_probs$time, 1-failure_probs$est, col="blue")
lines(failure_probs$time, 1-failure_probs$lcl, col="blue", lty=2)
lines(failure_probs$time, 1-failure_probs$ucl, col="blue", lty=2)
```

We also computed a traditional Kaplan-Meier curve treating the data as right censored only (assuming that participants joined the experiment about when their masks failed).
This is displayed in Figure \@ref(fig:comparekm), which shows much lower estimated early failure fractions as the later failures are no longer considered a possible early failure.

```{r echo=FALSE, fig.show="hold", warning=FALSE , message=FALSE, fig.keep="last", fig.cap="\\label{fig:comparekm}Comparison of interval censored and traditional survival estimates of mask failure by number of days worn. Black = non-parametric interval censored survival (grey regions = indeterminate MLE), Red = right censored only (Kaplan-Meier)"}
npm_out %>% plot(xlim=c(4,20), XLAB="Days worn", YLAB="Failure probability", dtype="cdf")

lines(km.ci(survfit(Surv(time=Days.Worn, event=fit.fail)~1, data=prelim.df) ), col='red', fun='F')
```

Similar analysis for failure by number of times donned is shown in Figure \@ref(fig:comparedon).
The failure fraction is similarly flat.
Number of times used was fairly correlated with number of days worn (spearman correlation = `r  with(prelim.df, cor(Uses, Days.Worn, method="spearman") )%>% round(2)`, p `r with(prelim.df, wilcox.test(Uses, Days.Worn))$p.value %>% format.pval(eps=.001)`).


```{r echo=FALSE, fig.show="hold", warning=FALSE , message=FALSE, fig.keep="last", fig.cap="\\label{fig:comparedon}Comparison of survival and cross sectional models for mask failure by number of times used. Black = non-parametric interval censored survival (grey regions = indeterminate MLE), Red = cross-sectional analysis, Blue = generalized gamma survival"}
npm_out2 %>% plot(xlim=c(6,50), XLAB="Times used", YLAB="Failure probability", dtype="cdf")
fs_1 <- flexsurvreg(Surv(left, right, type="interval2")~1, dist="gengamma", data=prelim.df2)
failure_probs<- summary(fs_1, type="survival", t=6:50, se=TRUE, ci=TRUE)[[1]]

lines(failure_probs$time, 1-failure_probs$est, col="blue")
lines(failure_probs$time, 1-failure_probs$lcl, col="blue", lty=2)
lines(failure_probs$time, 1-failure_probs$ucl, col="blue", lty=2)

cross_fit3 <- cgam(fit.fail ~ incr(Uses) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Uses = 6:55)
dummy_data <- cbind(dummy_data, predict(cross_fit3, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
lines(dummy_data$Uses, dummy_data$fit , col='red', lwd=2)
lines(dummy_data$Uses, dummy_data$lower, col='red', lty=2)
lines(dummy_data$Uses, dummy_data$upper, col='red', lty=2)
```
```{r echo=FALSE}
cross_fit <- glm(fit.fail ~ ns(Days.Worn) , family=binomial(), data=prelim.df )
cross_fit2 <- glm(fit.fail ~ ns(Days.Worn) +Sterilizations  , family=binomial(), data=prelim.df )
basic <- glm(fit.fail ~ Sterilizations , family=binomial(), data=prelim.df )
cross_fit3 <- glm(fit.fail ~ ns(Days.Worn) +I(Uses/Days.Worn)  , family=binomial(), data=prelim.df )

t1 <- np_reg %>% summary %>% extract2("summaryParameters") %>% as.vector %>% extract2(1) 
t2 <- np_reg %>% summary %>% extract2("summaryParameters") %>% as.vector %>% extract2(3) 
t3 <- np_reg2b %>% summary %>% extract2("summaryParameters") %>% as.vector %>% extract2(1)
t4 <- np_reg2b %>% summary %>% extract2("summaryParameters") %>% as.vector %>% extract2(3) 

```

Alternative factors contributing to fit failure analyzed included number of sterilizations and intensity of use (number of times donned per day). 
Using the logistic regression model, the number of sterilizations was found to have a modest effect with modest precision (OR `r cross_fit2 %>% coefficients %>% as.vector %>% extract2(3) %>% exp %>% round(1)` 95\% CI `r cross_fit2 %>% confint %>% magrittr::extract(3,1:2) %>% exp %>% round(1) %>% unname`).
The number of times donned per day was found to have a negligible effect with modest precision (OR `r cross_fit3 %>% coefficients %>% as.vector %>% extract2(3) %>% exp %>% round(1)` 95\% CI `r cross_fit3 %>% confint %>% magrittr::extract(3,1:2) %>% exp %>% round(1) %>% unname`).
In cox analysis, the number of sterilizations was found to have a small effect with modest precision (cox B =`r t1 %>% round(2)`, 95\% CI `r t1 %>% subtract(1.96*t2) %>% round(2) ` to  `r t1 %>% add(1.96*t2) %>% round(2) ` ).
The number of times donned per day was found to have a negligible effect with modest precision (cox B =`r t3 %>% round(2)`, 95\% CI `r t3 %>% subtract(1.96*t4) %>% round(2) ` to  `r t3 %>% add(1.96*t4) %>% round(2) ` ).


```{r echo=FALSE}
local_data <- prelim.df %>% group_by(Fits.well) %>% summarize(mfit=mean(fit.fail ), nfit=n(), lower=PropCIs::scoreci(x=sum(fit.fail), n=n(), conf.level=0.95)$conf.int[1], upper=PropCIs::scoreci(x=sum(fit.fail), n=n(), conf.level=0.95)$conf.int[2] )
local_data2 <- prelim.df %>%mutate(Fits.well = Fits.well == "No") %>% group_by(fit.fail) %>% summarize(mfit=mean(Fits.well ), nfit=n(), lower=PropCIs::scoreci(x=sum(Fits.well), n=n(), conf.level=0.95)$conf.int[1], upper=PropCIs::scoreci(x=sum(Fits.well), n=n(), conf.level=0.95)$conf.int[2] )
temp_conf <- with(prelim.df, caret::confusionMatrix(reference=factor(!fit.fail, labels=c("Yes", "No")), data=Fits.well %>% fct_recode(Yes="No", No="Yes" )))
local_data3 <- prelim.df %>% mutate(qual2=Mask.quality %in% c("Like New", "Good") ) %>% group_by(fit.fail) %>% summarize(mfit=mean(qual2 ), nfit=n(), lower=PropCIs::scoreci(x=sum(qual2), n=n(), conf.level=0.95)$conf.int[1], upper=PropCIs::scoreci(x=sum(qual2), n=n(), conf.level=0.95)$conf.int[2] )
local_data4 <- prelim.df %>% mutate(qual2=Mask.quality %in% c("Like New") ) %>% select(fit.fail, qual2 ) %>% table %>% fisher.test
```
Although imperfect, participants were able to somewhat discriminate poorly fitting masks.
Figure \@ref(fig:split) displays the survival stratified by perceived fit.
Among the `r local_data[1,3]` participants reporting a poor fit, `r local_data[1,2] %>% round(2) %>% multiply_by(100)`% failed the fit test (95% CI `r local_data[1,4]%>% round(2) %>% multiply_by(100)` to `r local_data[1,5]%>% round(2) %>% multiply_by(100)`).
Among the `r local_data[2,3]` participants reporting a good fit, `r local_data[2,2] %>% round(2)%>% multiply_by(100)`% failed the fit test (95% CI `r local_data[2,4]%>% round(2) %>% multiply_by(100)` to `r local_data[2,5]%>% round(2) %>% multiply_by(100)`, p `r fisher.test(prelim.df$Fits.well, prelim.df$fit.fail) %>% extract2("p.value") %>% format.pval(digits=2)` by Fisher's exact test).
Reversing the direction of conditioning, among those passing the fit test, `r local_data2[1,2] %>% round(2) %>% multiply_by(100)`% believed their mask was poorly fitting (95% CI `r local_data2[1,4]%>% round(2) %>% multiply_by(100)` to `r local_data2[1,5]%>% round(2) %>% multiply_by(100)`). 
Among those failing the fit test, `r local_data2[2,2] %>% round(2)%>% multiply_by(100)`% believed their mask to be poorly fitting (95% CI `r local_data2[2,4]%>% round(2) %>% multiply_by(100)` to `r local_data2[2,5]%>% round(2) %>% multiply_by(100)`).
The user's impression had a sensitivity of `r temp_conf[['byClass']]['Sensitivity'] %>% multiply_by(100) %>% round`%, specificity of `r temp_conf[['byClass']]['Specificity'] %>% multiply_by(100) %>% round`%, and positive and negative predictive values of  `r temp_conf[['byClass']]['Pos Pred Value'] %>% multiply_by(100) %>% round`% and  `r temp_conf[['byClass']]['Neg Pred Value'] %>% multiply_by(100) %>% round`%.
Test administrators also judged mask quality.
As seen in Table \@ref(tab:desc), very few masks were judged to be poor quality, but "Like New" and "Good" quality masks were `r local_data3[2,2] %>% round(2)%>% multiply_by(100)`% of the failed masks (95% CI `r local_data3[2,4]%>% round(2) %>% multiply_by(100)` to `r local_data3[2,5]%>% round(2) %>% multiply_by(100)`).
"Like New" masks were much less likely to fail, OR = `r local_data4$estimate %>% round(1)` (95% CI =`r local_data4$conf.int[1] %>% round(1)` to `r local_data4$conf.int[2] %>% round(1)`, p `r local_data4$p.value%>% format.pval(eps=0.001)`).

```{r echo=FALSE, fig.show="hold", warning=FALSE , message=FALSE, fig.keep="last", fig.cap="\\label{fig:split}Comparison of mask failure by days worn stratified by user perceived fit. Black = entire study non-parametric interval censored survival (grey regions = indeterminate MLE), Red= smooth cross-sectional model among self-perceived good fit, Blue = smooth cross-sectional model among self-perceived bad fit "}
cross_fit2 <- cgam(fit.fail ~ s.incr(Days.Worn)*Fits.well  , family=binomial(), data=prelim.df )

npm_out %>% plot(xlim=c(4,20), XLAB="Days worn", YLAB="Failure probability", dtype="cdf")

dummy_data <- data.frame(Days.Worn = 4:20 , Fits.well =prelim.df$Fits.well %>% unique %>% magrittr::extract(1) )
dummy_data <- cbind(dummy_data, predict(cross_fit2, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
lines(dummy_data$Days.Worn, dummy_data$fit , col='red', lwd=2)
lines(dummy_data$Days.Worn, dummy_data$lower, col='red', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='red', lty=2)

dummy_data <- data.frame(Days.Worn = 4:20 , Fits.well =prelim.df$Fits.well %>% unique %>% magrittr::extract(2) )
dummy_data <- cbind(dummy_data, predict(cross_fit2, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
lines(dummy_data$Days.Worn, dummy_data$fit , col='blue', lwd=2)
lines(dummy_data$Days.Worn, dummy_data$lower, col='blue', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='blue', lty=2)
```

<!--
# Conclusions

In this data, mask fit failure was high for all durations of use and varied minimally by duration and number of uses.
This surprising finding has several potential explanations.
First and most concerning, mask fit failure could occur more frequently early in the use cycle among those where it will fail.
Use patterns or facial morphology may tend to early failure, while others are able to maintain a good fit for long duration of use.
Alternatively, our results could be generated if fit test failure were poorly measured (false failures) or related to inappropriate donning during the test, which would produce a uniform failure fraction.
However, in our re-testing protocol individuals were given new masks immediately after a failed fit and almost uniformly passed the fit test, which makes isolated inappropriate donning or test error a less likely explanation.
Strong selection bias where individuals joined the study when they believed their mask failed could generate similar uniform failure fractions, but is not consistent with the majority of participants believing their mask fit was acceptable.
If clearly failed masks were removed from the population before the experiment, the failure fraction would tend to be artificially decreased (or made more uniform).
However, during the period of the study N95 mask shortages were critical, and discarding masks appears to have been uncommon.
Because few individuals trained in mask fitting were available, re-testing was not readily available outside of our initiative, and participants had little ability to verify that a mask fit had failed.
As seen in Figure \@ref(fig:hists), individuals continued to use masks for long durations even before sterilization was available due to these shortages and lack of testing availability.
-->

# References




    runs-on: ---
output: 
  bookdown::html_document2:
    fig_caption: true
    toc: false
    self_contained: no
title: "Web Appendix for Probability of Fit Failure with Reuse of N95 Respirators"
author: 
 - 
date: "`r format(Sys.time(), '%B %d, %Y')`"
bibliography: n95.bib

---

```{r echo=FALSE, message=FALSE, results='hide'}
library(magrittr)
if(file.exists("n95_intermediates.Rdata"))  {
load("n95_intermediates.Rdata")
library("magrittr")
library("tidyverse")
library("survival")
library("rmarkdown")
library("interval")
library("cgam")
library("icenReg")
library("km.ci")
library("flexsurv")
library("bookdown")
library("tableone")
library("PropCIs")
} else {
source("n95_report.R")
}
```

```{r setup, include=FALSE}
library('Cairo')
knitr::opts_chunk$set(dev = 'CairoSVG')
```



# Modeling assumptions and strategy
The sample is generated cross-sectionally, but can also be thought of as survival data.
Each fit test provides interval censored information: masks observed to fail a fit test at time *t* failed at some point in $\left(0 , t\right)$ (masks are assumed to have been initially functional). 
Masks that pass the fit test fail some time in the future $(t, t_\textrm{max})$.
For survival analysis we assume that each mask is independent with failure hazard from from a common distribution.
Our parametric survival model fits a 3-parameter generalized gamma hazard to the interval censored data.

Non-parametric or semi-parametric estimators which do not assume a functional form of the hazard are common in traditional survival analysis, and are available for interval censored data as well.
However, non-parametric estimates (NPMLE) for interval censored data are not unique; in some regions any baseline hazard matching continuity requirements is equally supported by the data.
Confidence intervals for interval censored baseline hazards are an active area of research [@wang_flexible_2016; @zhao_simultaneous_2019], with completely non-parametric estimation methods yielding slow $n^\frac{1}{3}$ convergence without a known asymptotic law [@groeneboom_information_1992].
Several semi-parametric methods have recently been shown to have $n^\frac{1}{2}$ convergence with Gaussian asymptotics making them amenable to bootstrapping [@wang_flexible_2016; @bouaziz_regression_2018; @zhao_simultaneous_2019; @li_adaptive_2019].
Proportional hazards regression was used to model factors other than duration of use in this framework.

Modeling the data as cross-sectional observations of fit passing versus failure as a function of time is also supportable because no participant-mask pair is observed more than once and there is minimal information to support extrapolation from before the earliest test time.
Because only a very small number of individuals are tested on more than one mask, we do not model or adjust for participant effects.
Integrated b-splines and constrained step functions allow modeling the failure log-odds as a monotone increasing function of time in a traditional logistic regression analysis.
This approach yields much better studied statistical behavior; however, given the modest sample size the probability statements produced should be taken as approximate.
The cross-sectional model does not encode any behavior before the earliest test time (e.g. that all masks initially fit).
We found that the cross-sectional model and non-parametric and semi-parametric survival models yield essentially identical point estimates when tuned appropriately, which is anticipated based on the use of step functions and splines to model the baseline hazard in the survival routines.

Factors beyond use duration were analyzed with proportional hazards regression (interval survival) or logistic regression (cross sectional).
Calculations were performed in R 3.6.3 with package flexsurv [@jackson_flexsurv_2016] for parametric survival modeling, icenReg [@anderson-bergman_icenreg_2017] for non-parametric survival modeling, and cgam [@liao_cgam_2019] for monotone logistic regression. 
Reported confidence intervals are at the 0.95 level.
Because this was an exploratory study, no pre-determined p-value threshold was applied.

# Results
```{r echo=FALSE}
cross_fit <- cgam(fit.fail ~ s.incr(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = c(4,10,15))
dummy_data <- cbind(dummy_data, predict(cross_fit, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
```

```{r desc, echo=FALSE, messages=FALSE, results='asis'}
factor_vars<- c('Sex' , 'Title' , 'Respirator', 'Fits.well', 'Mask.quality'  )
non_vars <- c( 'Days.Worn', 'Sterilizations', 'Uses' )
con_vars<- c( 'Days.Worn', 'Sterilizations', 'Uses' )
tab3 <- CreateTableOne(vars = c(con_vars,factor_vars) , strata = "fit.fail" , data = prelim.df%>%  mutate(fit.fail=factor(fit.fail, labels=c("Pass", "Fail")) )  , factorVars = factor_vars)
temp <- capture.output(x <- print(tab3 , showAllLevels = TRUE, contDigits=0, printToggle=FALSE, nonnormal=TRUE, exact=T))

tab2 <- CreateTableOne(vars ='Uses.per.day' , strata = "fit.fail" , data = prelim.df%>%  mutate( Uses.per.day=Uses/Days.Worn ) %>% mutate(fit.fail=factor(fit.fail, labels=c("Pass", "Fail")) )  )

temp <- capture.output(x2<- print(tab2 , showAllLevels = TRUE, contDigits=1, printToggle=FALSE, nonnormal=FALSE, exact=T))

x<- rbind(x[1:length(con_vars), ,drop=FALSE], x2[-1,,drop=FALSE], x[-c(1:length(con_vars)), ,drop=FALSE] )

x <- x[,seq(ncol(x)-1)]
rownames(x) <- gsub(rownames(x), pattern=".", fixed=TRUE, replacement=" " )
    knitr::kable(x, caption="Descriptive statistics stratified by fit test failure. P-values for quantitative variables by Mann-Whitney U, factor variables by Fisher's exact test.")
```

Table \@ref(tab:desc) provides descriptive statistics.
Notably, male participants were less likely to fail fit tests both marginally and adjusting for the number of days worn (OR = `r temp <- glm(fit.fail ~ ns(Days.Worn) + Sex, data=prelim.df); temp2<- temp %>% coef %>% exp %>% extract2(3) %>% round(1); temp2 <- paste(temp2, "95% CI", paste(temp%>% confint %>% exp %>% magrittr::extract(3,1:2) %>% round(1), collapse=" to ") ); temp2   `).
Figure \@ref(fig:hists) displays histograms of number of days worn, number of times used, and number of times sterilized.
Surprisingly, the use duration and intensity was not markedly different between failed and passed mask fits.
Breaking the data into groups by number of days worn, the failure fraction is displayed in Table \@ref(tab:broken).
Table \@ref(tab:broken) shows that the failure fraction at all times is high; approximately 50% in all time categories.

```{r hists, echo=FALSE, fig.show="hold", fig.keep="last", fig.cap="Mask use histograms."}
par(mfrow=c(3,1), oma=c(0,0,0,0), mar=c(2, 3, 2,0))
hist(prelim.df  %>% select(Days.Worn) %>% unlist ,breaks = seq(from=0.5, to=60.5), plot=T, main="Days Worn", xlab=NULL, col= rgb(0.6,1.0,1.0,alpha=0.5))
hist(prelim.df  %>% select(Uses) %>% unlist , breaks=20, plot=T, main="Times used", xlab=NULL, col= rgb(0.6,1.0,1.0,alpha=0.5))
hist(prelim.df  %>% select(Sterilizations) %>% unlist ,  plot=T, main="Times sterilized", xlab=NULL, col= rgb(0.6,1.0,1.0,alpha=0.5))
par(mfrow=c(1,1))
```


```{r broken, echo=FALSE, results='asis' }
prelim.df %>% mutate(day_group = cut(Days.Worn, breaks=c(0.5, 5, 8, 12, 61)) ) %>% group_by(day_group) %>% summarize(n=n() , mean.failure=mean(fit.fail )%>% round(2) %>% multiply_by(100), lower.conf= paste( "(" , PropCIs::scoreci(x=sum(fit.fail), n=n(), conf.level=0.95)$conf.int[1]%>% round(2)%>% multiply_by(100), ",", PropCIs::scoreci(x=sum(fit.fail), n=n(), conf.level=0.95)$conf.int[2]%>% round(2)%>% multiply_by(100) , ")" ) ) %>% set_colnames(c("days worn", "n", "failed %", "CI")) %>% {knitr::kable(x=., align="c", format="html", caption="Failure fractions by duration worn. Confidence intervals by Wilson's score method. ")}
```

Figure \@ref(fig:basicsurvival) displays mask survival as a function of days worn using cross-sectional approaches.
A monotone fit (I-splines) and natural spline (no restrictions) can be seen to agree well with non-parametric monotone methods.
The failure fraction at day `r dummy_data[1, "Days.Worn"]%>% as.integer` is `r dummy_data[1, "fit"]%>% round(2)` (95\% CI `r round(dummy_data[1,c("lower", "upper")],2)`). At day `r dummy_data[2, "Days.Worn"]%>% as.integer` the failure has increased to `r dummy_data[2, "fit"]%>% round(2)` (95\% CI `r round(dummy_data[2,c("lower", "upper")],2)`) and day `r dummy_data[3, "Days.Worn"]%>% as.integer` to `r dummy_data[3, "fit"]%>% round(2)` (95\% CI `r round(dummy_data[3,c("lower", "upper")],2)`).


```{r basicsurvival, echo=FALSE, fig.show="hold", fig.keep="last", fig.cap="Comparison of cross sectional logistic regression models for mask Failure by number of days worn. Black = non-parametric monotone (step functions), Red = semi-parametric smooth with monotone constraint, Blue = semi-parametric smooth (no monotone requirement). Dashed lines 95% point-wise confidence limits. Fit with cgam package."}
cross_fit <- cgam(fit.fail ~ incr(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = 4:20)
dummy_data <- cbind(dummy_data, predict(cross_fit, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
plot(dummy_data$Days.Worn, dummy_data$fit , xlab="Days worn", ylab="Failure probability", ylim=c(0,1), xlim=c(4,20), type="l")
lines(dummy_data$Days.Worn, dummy_data$lower, lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, lty=2)

cross_fit <- cgam(fit.fail ~ s.incr(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = 4:20)
dummy_data <- cbind(dummy_data, predict(cross_fit, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
points(dummy_data$Days.Worn, dummy_data$fit , col='red', type="l")
lines(dummy_data$Days.Worn, dummy_data$lower, col='red', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='red', lty=2)


cross_fit <- glm(fit.fail ~ ns(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = 4:20)
dummy_data <- cbind(dummy_data, predict(cross_fit, newdata=dummy_data, se.fit=TRUE)[c("fit", "se.fit")] %>% as.data.frame )

points(dummy_data$Days.Worn, cross_fit$family$linkinv(dummy_data$fit) , col='blue', type="l")
dummy_data %<>% mutate( lower=cross_fit$family$linkinv(fit - 1.96*se.fit), upper=cross_fit$family$linkinv(fit+1.96*se.fit))
lines(dummy_data$Days.Worn, dummy_data$lower, col='blue', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='blue', lty=2)
```

Figure \@ref(fig:comparesurv) compares interval censored survival-based analyses and the cross-sectional analysis.
The parametric model diverges from the non-parametric MLE at the extremes of time, as it requires a smoother change between the initial state (0 failure) to the early observed times.
The generalized gamma and Weibull parametric families fit well.
Figure \@ref(fig:comparepara) displays the results with gamma and weibull hazards.


```{r comparesurv, echo=FALSE, fig.show="hold", fig.keep="last", fig.cap="Comparison of survival and cross-sectional models for mask failure by number of days worn. Black = non-parametric survival with interval censoring (grey regions = indeterminate MLE), Red = generalized gamma survival model with interval censoring, Blue = cross sectional monotone smooth. Dashed lines 95% point-wise confidence limits. "}
npm_out %>% plot(xlim=c(4,20), XLAB="Days worn", YLAB="Failure probability", dtype="cdf")
fs_1 <- flexsurvreg(Surv(left, right, type="interval2")~1, data=prelim.df, dist="gengamma")
cross_fit <- cgam(fit.fail ~ s.incr(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = 4:20)
dummy_data <- cbind(dummy_data, predict(cross_fit, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
lines(dummy_data$Days.Worn, dummy_data$fit , col='blue', lwd=2)
lines(dummy_data$Days.Worn, dummy_data$lower, col='blue', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='blue', lty=2)

failure_probs<- summary(fs_1, type="survival", t=4:20, se=TRUE, ci=TRUE)[[1]]

lines(failure_probs$time, 1-failure_probs$est, col="red")
lines(failure_probs$time, 1-failure_probs$lcl, col="red", lty=2)
lines(failure_probs$time, 1-failure_probs$ucl, col="red", lty=2)
```

```{r comparepara, echo=FALSE, fig.show="hold", warning=FALSE , message=FALSE, fig.keep="last", fig.cap="Comparison of parametric family survival models. Black = non-parametric survival (grey regions = indeterminate MLE), Red = generalized gamma survival, Blue = Weibull. Dashed lines 95% point-wise confidence limits. Fit with cgam package."}
npm_out %>% plot(xlim=c(4,20), XLAB="Days worn", YLAB="Failure probability", dtype="cdf")
lines(failure_probs$time, 1-failure_probs$est, col="red")
lines(failure_probs$time, 1-failure_probs$lcl, col="red", lty=2)
lines(failure_probs$time, 1-failure_probs$ucl, col="red", lty=2)


fs_1 <- flexsurvreg(Surv(left, right, type="interval2")~1, data = prelim.df, dist="weibull")

failure_probs<- summary(fs_1, type="survival", t=4:20, se=TRUE, ci=TRUE)[[1]]
lines(failure_probs$time, 1-failure_probs$est, col="blue")
lines(failure_probs$time, 1-failure_probs$lcl, col="blue", lty=2)
lines(failure_probs$time, 1-failure_probs$ucl, col="blue", lty=2)
```

We also computed a traditional Kaplan-Meier curve treating the data as right censored only (assuming that participants joined the experiment about when their masks failed).
This is displayed in Figure \@ref(fig:comparekm), which shows much lower estimated early failure fractions as the later failures are no longer considered a possible early failure.

```{r comparekm, echo=FALSE, fig.show="hold", warning=FALSE , message=FALSE, fig.keep="last", fig.cap="Comparison of interval censored and traditional survival estimates of mask failure by number of days worn. Black = non-parametric interval censored survival (grey regions = indeterminate MLE), Red = right censored only (Kaplan-Meier)"}
npm_out %>% plot(xlim=c(4,20), XLAB="Days worn", YLAB="Failure probability", dtype="cdf")

lines(km.ci(survfit(Surv(time=Days.Worn, event=fit.fail)~1, data=prelim.df) ), col='red', fun='F')
```

Similar analysis for failure by number of times donned is shown in Figure \@ref(fig:comparedon).
The failure fraction is similarly flat.
Number of times used was fairly correlated with number of days worn (spearman correlation = `r  with(prelim.df, cor(Uses, Days.Worn, method="spearman") )%>% round(2)`, p `r with(prelim.df, wilcox.test(Uses, Days.Worn))$p.value %>% format.pval(eps=.001)`).


```{r comparedon, echo=FALSE, fig.show="hold", warning=FALSE , message=FALSE, fig.keep="last", fig.cap="Comparison of survival and cross sectional models for mask failure by number of times used. Black = non-parametric interval censored survival (grey regions = indeterminate MLE), Red = cross-sectional analysis, Blue = generalized gamma survival"}
npm_out2 %>% plot(xlim=c(6,50), XLAB="Times used", YLAB="Failure probability", dtype="cdf")
fs_1 <- flexsurvreg(Surv(left, right, type="interval2")~1, dist="gengamma", data=prelim.df2)
failure_probs<- summary(fs_1, type="survival", t=6:50, se=TRUE, ci=TRUE)[[1]]

lines(failure_probs$time, 1-failure_probs$est, col="blue")
lines(failure_probs$time, 1-failure_probs$lcl, col="blue", lty=2)
lines(failure_probs$time, 1-failure_probs$ucl, col="blue", lty=2)

cross_fit3 <- cgam(fit.fail ~ incr(Uses) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Uses = 6:55)
dummy_data <- cbind(dummy_data, predict(cross_fit3, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
lines(dummy_data$Uses, dummy_data$fit , col='red', lwd=2)
lines(dummy_data$Uses, dummy_data$lower, col='red', lty=2)
lines(dummy_data$Uses, dummy_data$upper, col='red', lty=2)
```
```{r echo=FALSE}
cross_fit <- glm(fit.fail ~ ns(Days.Worn) , family=binomial(), data=prelim.df )
cross_fit2 <- glm(fit.fail ~ ns(Days.Worn) +Sterilizations  , family=binomial(), data=prelim.df )
basic <- glm(fit.fail ~ Sterilizations , family=binomial(), data=prelim.df )
cross_fit3 <- glm(fit.fail ~ ns(Days.Worn) +I(Uses/Days.Worn)  , family=binomial(), data=prelim.df )

t1 <- np_reg %>% summary %>% extract2("summaryParameters") %>% as.vector %>% extract2(1) 
t2 <- np_reg %>% summary %>% extract2("summaryParameters") %>% as.vector %>% extract2(3) 
t3 <- np_reg2b %>% summary %>% extract2("summaryParameters") %>% as.vector %>% extract2(1)
t4 <- np_reg2b %>% summary %>% extract2("summaryParameters") %>% as.vector %>% extract2(3) 

```

Alternative factors contributing to fit failure analyzed included number of sterilizations and intensity of use (number of times donned per day). 
Using the logistic regression model, the number of sterilizations was found to have a modest effect with modest precision (OR `r cross_fit2 %>% coefficients %>% as.vector %>% extract2(3) %>% exp %>% round(1)` 95\% CI `r cross_fit2 %>% confint %>% magrittr::extract(3,1:2) %>% exp %>% round(1) %>% unname`).
The number of times donned per day was found to have a negligible effect with modest precision (OR `r cross_fit3 %>% coefficients %>% as.vector %>% extract2(3) %>% exp %>% round(1)` 95\% CI `r cross_fit3 %>% confint %>% magrittr::extract(3,1:2) %>% exp %>% round(1) %>% unname`).
In cox analysis, the number of sterilizations was found to have a small effect with modest precision (cox B =`r t1 %>% round(2)`, 95\% CI `r t1 %>% subtract(1.96*t2) %>% round(2) ` to  `r t1 %>% add(1.96*t2) %>% round(2) ` ).
The number of times donned per day was found to have a negligible effect with modest precision (cox B =`r t3 %>% round(2)`, 95\% CI `r t3 %>% subtract(1.96*t4) %>% round(2) ` to  `r t3 %>% add(1.96*t4) %>% round(2) ` ).


```{r echo=FALSE}
local_data <- prelim.df %>% group_by(Fits.well) %>% summarize(mfit=mean(fit.fail ), nfit=n(), lower=PropCIs::scoreci(x=sum(fit.fail), n=n(), conf.level=0.95)$conf.int[1], upper=PropCIs::scoreci(x=sum(fit.fail), n=n(), conf.level=0.95)$conf.int[2] )
local_data2 <- prelim.df %>%mutate(Fits.well = Fits.well == "No") %>% group_by(fit.fail) %>% summarize(mfit=mean(Fits.well ), nfit=n(), lower=PropCIs::scoreci(x=sum(Fits.well), n=n(), conf.level=0.95)$conf.int[1], upper=PropCIs::scoreci(x=sum(Fits.well), n=n(), conf.level=0.95)$conf.int[2] )
temp_conf <- with(prelim.df, caret::confusionMatrix(reference=factor(!fit.fail, labels=c("Yes", "No")), data=Fits.well %>% fct_recode(Yes="No", No="Yes" )))
local_data3 <- prelim.df %>% mutate(qual2=Mask.quality %in% c("Like New", "Good") ) %>% group_by(fit.fail) %>% summarize(mfit=mean(qual2 ), nfit=n(), lower=PropCIs::scoreci(x=sum(qual2), n=n(), conf.level=0.95)$conf.int[1], upper=PropCIs::scoreci(x=sum(qual2), n=n(), conf.level=0.95)$conf.int[2] )
local_data4 <- prelim.df %>% mutate(qual2=Mask.quality %in% c("Like New") ) %>% select(fit.fail, qual2 ) %>% table %>% fisher.test
```
Although imperfect, participants were able to somewhat discriminate poorly fitting masks.
Figure \@ref(fig:split) displays the survival stratified by perceived fit.
Among the `r local_data[1,3]` participants reporting a poor fit, `r local_data[1,2] %>% round(2) %>% multiply_by(100)`% failed the fit test (95% CI `r local_data[1,4]%>% round(2) %>% multiply_by(100)` to `r local_data[1,5]%>% round(2) %>% multiply_by(100)`).
Among the `r local_data[2,3]` participants reporting a good fit, `r local_data[2,2] %>% round(2)%>% multiply_by(100)`% failed the fit test (95% CI `r local_data[2,4]%>% round(2) %>% multiply_by(100)` to `r local_data[2,5]%>% round(2) %>% multiply_by(100)`, p `r fisher.test(prelim.df$Fits.well, prelim.df$fit.fail) %>% extract2("p.value") %>% format.pval(digits=2)` by Fisher's exact test).
Reversing the direction of conditioning, among those passing the fit test, `r local_data2[1,2] %>% round(2) %>% multiply_by(100)`% believed their mask was poorly fitting (95% CI `r local_data2[1,4]%>% round(2) %>% multiply_by(100)` to `r local_data2[1,5]%>% round(2) %>% multiply_by(100)`). 
Among those failing the fit test, `r local_data2[2,2] %>% round(2)%>% multiply_by(100)`% believed their mask to be poorly fitting (95% CI `r local_data2[2,4]%>% round(2) %>% multiply_by(100)` to `r local_data2[2,5]%>% round(2) %>% multiply_by(100)`).
The user's impression had a sensitivity of `r temp_conf[['byClass']]['Sensitivity'] %>% multiply_by(100) %>% round`%, specificity of `r temp_conf[['byClass']]['Specificity'] %>% multiply_by(100) %>% round`%, and positive and negative predictive values of  `r temp_conf[['byClass']]['Pos Pred Value'] %>% multiply_by(100) %>% round`% and  `r temp_conf[['byClass']]['Neg Pred Value'] %>% multiply_by(100) %>% round`%.
Test administrators also judged mask quality.
As seen in Table \@ref(tab:desc), very few masks were judged to be poor quality, but "Like New" and "Good" quality masks were `r local_data3[2,2] %>% round(2)%>% multiply_by(100)`% of the failed masks (95% CI `r local_data3[2,4]%>% round(2) %>% multiply_by(100)` to `r local_data3[2,5]%>% round(2) %>% multiply_by(100)`).
"Like New" masks were much less likely to fail, OR = `r local_data4$estimate %>% round(1)` (95% CI =`r local_data4$conf.int[1] %>% round(1)` to `r local_data4$conf.int[2] %>% round(1)`, p `r local_data4$p.value%>% format.pval(eps=0.001)`).

```{r split, echo=FALSE, fig.show="hold", warning=FALSE , message=FALSE, fig.keep="last", fig.cap="Comparison of mask failure by days worn stratified by user perceived fit. Black = entire study non-parametric interval censored survival (grey regions = indeterminate MLE), Red= smooth cross-sectional model among self-perceived good fit, Blue = smooth cross-sectional model among self-perceived bad fit "}
cross_fit2 <- cgam(fit.fail ~ s.incr(Days.Worn)*Fits.well  , family=binomial(), data=prelim.df )

npm_out %>% plot(xlim=c(4,20), XLAB="Days worn", YLAB="Failure probability", dtype="cdf")

dummy_data <- data.frame(Days.Worn = 4:20 , Fits.well =prelim.df$Fits.well %>% unique %>% magrittr::extract(1) )
dummy_data <- cbind(dummy_data, predict(cross_fit2, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
lines(dummy_data$Days.Worn, dummy_data$fit , col='red', lwd=2)
lines(dummy_data$Days.Worn, dummy_data$lower, col='red', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='red', lty=2)

dummy_data <- data.frame(Days.Worn = 4:20 , Fits.well =prelim.df$Fits.well %>% unique %>% magrittr::extract(2) )
dummy_data <- cbind(dummy_data, predict(cross_fit2, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
lines(dummy_data$Days.Worn, dummy_data$fit , col='blue', lwd=2)
lines(dummy_data$Days.Worn, dummy_data$lower, col='blue', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='blue', lty=2)
```

<!--
# Conclusions

In this data, mask fit failure was high for all durations of use and varied minimally by duration and number of uses.
This surprising finding has several potential explanations.
First and most concerning, mask fit failure could occur more frequently early in the use cycle among those where it will fail.
Use patterns or facial morphology may tend to early failure, while others are able to maintain a good fit for long duration of use.
Alternatively, our results could be generated if fit test failure were poorly measured (false failures) or related to inappropriate donning during the test, which would produce a uniform failure fraction.
However, in our re-testing protocol individuals were given new masks immediately after a failed fit and almost uniformly passed the fit test, which makes isolated inappropriate donning or test error a less likely explanation.
Strong selection bias where individuals joined the study when they believed their mask failed could generate similar uniform failure fractions, but is not consistent with the majority of participants believing their mask fit was acceptable.
If clearly failed masks were removed from the population before the experiment, the failure fraction would tend to be artificially decreased (or made more uniform).
However, during the period of the study N95 mask shortages were critical, and discarding masks appears to have been uncommon.
Because few individuals trained in mask fitting were available, re-testing was not readily available outside of our initiative, and participants had little ability to verify that a mask fit had failed.
As seen in Figure \@ref(fig:hists), individuals continued to use masks for long durations even before sterilization was available due to these shortages and lack of testing availability.
-->

# References




    services: ---
output: 
  bookdown::word_document2:
    fig_caption: true
    toc: false
title: "Web Appendix for Probability of Fit Failure with Reuse of N95 Respirators"
author: 
 -

date: "`r format(Sys.time(), '%B %d, %Y')`"
bibliography: n95.bib
---

```{r echo=FALSE, message=FALSE, results='hide'}
library(magrittr)
if(file.exists("n95_intermediates.Rdata"))  {
load("n95_intermediates.Rdata")
library("magrittr")
library("tidyverse")
library("survival")
library("rmarkdown")
library("interval")
library("cgam")
library("icenReg")
library("km.ci")
library("flexsurv")
library("bookdown")
library("tableone")
library("PropCIs")
} else {
source("n95_report.R")
}
```

```{r setup, include=FALSE}
knitr::opts_chunk$set(dev = 'pdf')
```



# Modeling assumptions and strategy
The sample is generated cross-sectionally, but can also be thought of as survival data.
Each fit test provides interval censored information: masks observed to fail a fit test at time *t* failed at some point in $\left(0 , t\right)$ (masks are assumed to have been initially functional). 
Masks that pass the fit test fail some time in the future $(t, t_\textrm{max})$.
For survival analysis we assume that each mask is independent with failure hazard from from a common distribution.
Our parametric survival model fits a 3-parameter generalized gamma hazard to the interval censored data.

Non-parametric or semi-parametric estimators which do not assume a functional form of the hazard are common in traditional survival analysis, and are available for interval censored data as well.
However, non-parametric estimates (NPMLE) for interval censored data are not unique; in some regions any baseline hazard matching continuity requirements is equally supported by the data.
Confidence intervals for interval censored baseline hazards are an active area of research [@wang_flexible_2016; @zhao_simultaneous_2019], with completely non-parametric estimation methods yielding slow $n^\frac{1}{3}$ convergence without a known asymptotic law [@groeneboom_information_1992].
Several semi-parametric methods have recently been shown to have $n^\frac{1}{2}$ convergence with Gaussian asymptotics making them amenable to bootstrapping [@wang_flexible_2016; @bouaziz_regression_2018; @zhao_simultaneous_2019; @li_adaptive_2019].
Proportional hazards regression was used to model factors other than duration of use in this framework.

Modeling the data as cross-sectional observations of fit passing versus failure as a function of time is also supportable because no participant-mask pair is observed more than once and there is minimal information to support extrapolation from before the earliest test time.
Because only a very small number of individuals are tested on more than one mask, we do not model or adjust for participant effects.
Integrated b-splines and constrained step functions allow modeling the failure log-odds as a monotone increasing function of time in a traditional logistic regression analysis.
This approach yields much better studied statistical behavior; however, given the modest sample size the probability statements produced should be taken as approximate.
The cross-sectional model does not encode any behavior before the earliest test time (e.g. that all masks initially fit).
We found that the cross-sectional model and non-parametric and semi-parametric survival models yield essentially identical point estimates when tuned appropriately, which is anticipated based on the use of step functions and splines to model the baseline hazard in the survival routines.

Factors beyond use duration were analyzed with proportional hazards regression (interval survival) or logistic regression (cross sectional).
Calculations were performed in R 3.6.3 with package flexsurv [@jackson_flexsurv_2016] for parametric survival modeling, icenReg [@anderson-bergman_icenreg_2017] for non-parametric survival modeling, and cgam [@liao_cgam_2019] for monotone logistic regression. 
Reported confidence intervals are at the 0.95 level.
Because this was an exploratory study, no pre-determined p-value threshold was applied.

# Results
```{r echo=FALSE}
cross_fit <- cgam(fit.fail ~ s.incr(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = c(4,10,15))
dummy_data <- cbind(dummy_data, predict(cross_fit, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
```

```{r desc, echo=FALSE, messages=FALSE, results='asis'}
factor_vars<- c('Sex' , 'Title' , 'Respirator', 'Fits.well', 'Mask.quality'  )
non_vars <- c( 'Days.Worn', 'Sterilizations', 'Uses' )
con_vars<- c( 'Days.Worn', 'Sterilizations', 'Uses' )
tab3 <- CreateTableOne(vars = c(con_vars,factor_vars) , strata = "fit.fail" , data = prelim.df%>%  mutate(fit.fail=factor(fit.fail, labels=c("Pass", "Fail")) )  , factorVars = factor_vars)
temp <- capture.output(x <- print(tab3 , showAllLevels = TRUE, contDigits=0, printToggle=FALSE, nonnormal=TRUE, exact=T))

tab2 <- CreateTableOne(vars ='Uses.per.day' , strata = "fit.fail" , data = prelim.df%>%  mutate( Uses.per.day=Uses/Days.Worn ) %>% mutate(fit.fail=factor(fit.fail, labels=c("Pass", "Fail")) )  )

temp <- capture.output(x2<- print(tab2 , showAllLevels = TRUE, contDigits=1, printToggle=FALSE, nonnormal=FALSE, exact=T))

x<- rbind(x[1:length(con_vars), ,drop=FALSE], x2[-1,,drop=FALSE], x[-c(1:length(con_vars)), ,drop=FALSE] )

x <- x[,seq(ncol(x)-1)]
rownames(x) <- gsub(rownames(x), pattern=".", fixed=TRUE, replacement=" " )
    knitr::kable(x, caption="Descriptive statistics stratified by fit test failure. P-values for quantitative variables by Mann-Whitney U, factor variables by Fisher's exact test.", format='pandoc')
```

Table \@ref(tab:desc) provides descriptive statistics.
Notably, male participants were less likely to fail fit tests both marginally and adjusting for the number of days worn (OR = `r temp <- glm(fit.fail ~ ns(Days.Worn) + Sex, data=prelim.df); temp2<- temp %>% coef %>% exp %>% extract2(3) %>% round(1); temp2 <- paste(temp2, "95% CI", paste(temp%>% confint %>% exp %>% magrittr::extract(3,1:2) %>% round(1), collapse=" to ") ); temp2   `).
Figure \@ref(fig:hists) displays histograms of number of days worn, number of times used, and number of times sterilized.
Surprisingly, the use duration and intensity was not markedly different between failed and passed mask fits.
Breaking the data into groups by number of days worn, the failure fraction is displayed in Table \@ref(tab:broken).
Table \@ref(tab:broken) shows that the failure fraction at all times is high; approximately 50% in all time categories.

```{r hists, echo=FALSE, fig.show="hold", fig.keep="last", fig.cap="Mask use histograms."}
par(mfrow=c(3,1), oma=c(0,0,0,0), mar=c(2, 3, 2,0))
hist(prelim.df  %>% select(Days.Worn) %>% unlist ,breaks = seq(from=0.5, to=60.5), plot=T, main="Days Worn", xlab=NULL, col= rgb(0.6,1.0,1.0,alpha=0.5))
hist(prelim.df  %>% select(Uses) %>% unlist , breaks=20, plot=T, main="Times used", xlab=NULL, col= rgb(0.6,1.0,1.0,alpha=0.5))
hist(prelim.df  %>% select(Sterilizations) %>% unlist ,  plot=T, main="Times sterilized", xlab=NULL, col= rgb(0.6,1.0,1.0,alpha=0.5))
par(mfrow=c(1,1))
```


```{r broken, echo=FALSE, results='asis' }
prelim.df %>% mutate(day_group = cut(Days.Worn, breaks=c(0.5, 5, 8, 12, 61)) ) %>% group_by(day_group) %>% summarize(n=n() , mean.failure=mean(fit.fail )%>% round(2) %>% multiply_by(100), lower.conf= paste( "(" , PropCIs::scoreci(x=sum(fit.fail), n=n(), conf.level=0.95)$conf.int[1]%>% round(2)%>% multiply_by(100), ",", PropCIs::scoreci(x=sum(fit.fail), n=n(), conf.level=0.95)$conf.int[2]%>% round(2)%>% multiply_by(100) , ")" ) ) %>% set_colnames(c("days worn", "n", "failed %", "CI")) %>% {knitr::kable(x=., align="c", format="pandoc", caption="Failure fractions by duration worn. Confidence intervals by Wilson's score method. ")}
```

Figure \@ref(fig:basicsurvival) displays mask survival as a function of days worn using cross-sectional approaches.
A monotone fit (I-splines) and natural spline (no restrictions) can be seen to agree well with non-parametric monotone methods.
The failure fraction at day `r dummy_data[1, "Days.Worn"]%>% as.integer` is `r dummy_data[1, "fit"]%>% round(2)` (95\% CI `r round(dummy_data[1,c("lower", "upper")],2)`). At day `r dummy_data[2, "Days.Worn"]%>% as.integer` the failure has increased to `r dummy_data[2, "fit"]%>% round(2)` (95\% CI `r round(dummy_data[2,c("lower", "upper")],2)`) and day `r dummy_data[3, "Days.Worn"]%>% as.integer` to `r dummy_data[3, "fit"]%>% round(2)` (95\% CI `r round(dummy_data[3,c("lower", "upper")],2)`).


```{r basicsurvival, echo=FALSE, fig.show="hold", fig.keep="last", fig.cap="Comparison of cross sectional logistic regression models for mask Failure by number of days worn. Black = non-parametric monotone (step functions), Red = semi-parametric smooth with monotone constraint, Blue = semi-parametric smooth (no monotone requirement). Dashed lines 95% point-wise confidence limits. Fit with cgam package."}
cross_fit <- cgam(fit.fail ~ incr(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = 4:20)
dummy_data <- cbind(dummy_data, predict(cross_fit, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
plot(dummy_data$Days.Worn, dummy_data$fit , xlab="Days worn", ylab="Failure probability", ylim=c(0,1), xlim=c(4,20), type="l")
lines(dummy_data$Days.Worn, dummy_data$lower, lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, lty=2)

cross_fit <- cgam(fit.fail ~ s.incr(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = 4:20)
dummy_data <- cbind(dummy_data, predict(cross_fit, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
points(dummy_data$Days.Worn, dummy_data$fit , col='red', type="l")
lines(dummy_data$Days.Worn, dummy_data$lower, col='red', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='red', lty=2)


cross_fit <- glm(fit.fail ~ ns(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = 4:20)
dummy_data <- cbind(dummy_data, predict(cross_fit, newdata=dummy_data, se.fit=TRUE)[c("fit", "se.fit")] %>% as.data.frame )

points(dummy_data$Days.Worn, cross_fit$family$linkinv(dummy_data$fit) , col='blue', type="l")
dummy_data %<>% mutate( lower=cross_fit$family$linkinv(fit - 1.96*se.fit), upper=cross_fit$family$linkinv(fit+1.96*se.fit))
lines(dummy_data$Days.Worn, dummy_data$lower, col='blue', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='blue', lty=2)
```

Figure \@ref(fig:comparesurv) compares interval censored survival-based analyses and the cross-sectional analysis.
The parametric model diverges from the non-parametric MLE at the extremes of time, as it requires a smoother change between the initial state (0 failure) to the early observed times.
The generalized gamma and Weibull parametric families fit well.
Figure \@ref(fig:comparepara) displays the results with gamma and weibull hazards.


```{r comparesurv, echo=FALSE, fig.show="hold", fig.keep="last", fig.cap="Comparison of survival and cross-sectional models for mask failure by number of days worn. Black = non-parametric survival with interval censoring (grey regions = indeterminate MLE), Red = generalized gamma survival model with interval censoring, Blue = cross sectional monotone smooth. Dashed lines 95% point-wise confidence limits. "}
npm_out %>% plot(xlim=c(4,20), XLAB="Days worn", YLAB="Failure probability", dtype="cdf")
fs_1 <- flexsurvreg(Surv(left, right, type="interval2")~1, data=prelim.df, dist="gengamma")
cross_fit <- cgam(fit.fail ~ s.incr(Days.Worn) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Days.Worn = 4:20)
dummy_data <- cbind(dummy_data, predict(cross_fit, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
lines(dummy_data$Days.Worn, dummy_data$fit , col='blue', lwd=2)
lines(dummy_data$Days.Worn, dummy_data$lower, col='blue', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='blue', lty=2)

failure_probs<- summary(fs_1, type="survival", t=4:20, se=TRUE, ci=TRUE)[[1]]

lines(failure_probs$time, 1-failure_probs$est, col="red")
lines(failure_probs$time, 1-failure_probs$lcl, col="red", lty=2)
lines(failure_probs$time, 1-failure_probs$ucl, col="red", lty=2)
```

```{r comparepara, echo=FALSE, fig.show="hold", warning=FALSE , message=FALSE, fig.keep="last", fig.cap="Comparison of parametric family survival models. Black = non-parametric survival (grey regions = indeterminate MLE), Red = generalized gamma survival, Blue = Weibull. Dashed lines 95% point-wise confidence limits. Fit with cgam package."}
npm_out %>% plot(xlim=c(4,20), XLAB="Days worn", YLAB="Failure probability", dtype="cdf")
lines(failure_probs$time, 1-failure_probs$est, col="red")
lines(failure_probs$time, 1-failure_probs$lcl, col="red", lty=2)
lines(failure_probs$time, 1-failure_probs$ucl, col="red", lty=2)


fs_1 <- flexsurvreg(Surv(left, right, type="interval2")~1, data = prelim.df, dist="weibull")

failure_probs<- summary(fs_1, type="survival", t=4:20, se=TRUE, ci=TRUE)[[1]]
lines(failure_probs$time, 1-failure_probs$est, col="blue")
lines(failure_probs$time, 1-failure_probs$lcl, col="blue", lty=2)
lines(failure_probs$time, 1-failure_probs$ucl, col="blue", lty=2)
```

We also computed a traditional Kaplan-Meier curve treating the data as right censored only (assuming that participants joined the experiment about when their masks failed).
This is displayed in Figure \@ref(fig:comparekm), which shows much lower estimated early failure fractions as the later failures are no longer considered a possible early failure.

```{r comparekm, echo=FALSE, fig.show="hold", warning=FALSE , message=FALSE, fig.keep="last", fig.cap="Comparison of interval censored and traditional survival estimates of mask failure by number of days worn. Black = non-parametric interval censored survival (grey regions = indeterminate MLE), Red = right censored only (Kaplan-Meier)"}
npm_out %>% plot(xlim=c(4,20), XLAB="Days worn", YLAB="Failure probability", dtype="cdf")

lines(km.ci(survfit(Surv(time=Days.Worn, event=fit.fail)~1, data=prelim.df) ), col='red', fun='F')
```

Similar analysis for failure by number of times donned is shown in Figure \@ref(fig:comparedon).
The failure fraction is similarly flat.
Number of times used was fairly correlated with number of days worn (spearman correlation = `r  with(prelim.df, cor(Uses, Days.Worn, method="spearman") )%>% round(2)`, p `r with(prelim.df, wilcox.test(Uses, Days.Worn))$p.value %>% format.pval(eps=.001)`).


```{r comparedon, echo=FALSE, fig.show="hold", warning=FALSE , message=FALSE, fig.keep="last", fig.cap="Comparison of survival and cross sectional models for mask failure by number of times used. Black = non-parametric interval censored survival (grey regions = indeterminate MLE), Red = cross-sectional analysis, Blue = generalized gamma survival"}
npm_out2 %>% plot(xlim=c(6,50), XLAB="Times used", YLAB="Failure probability", dtype="cdf")
fs_1 <- flexsurvreg(Surv(left, right, type="interval2")~1, dist="gengamma", data=prelim.df2)
failure_probs<- summary(fs_1, type="survival", t=6:50, se=TRUE, ci=TRUE)[[1]]

lines(failure_probs$time, 1-failure_probs$est, col="blue")
lines(failure_probs$time, 1-failure_probs$lcl, col="blue", lty=2)
lines(failure_probs$time, 1-failure_probs$ucl, col="blue", lty=2)

cross_fit3 <- cgam(fit.fail ~ incr(Uses) , family=binomial(), data=prelim.df )
dummy_data <- data.frame(Uses = 6:55)
dummy_data <- cbind(dummy_data, predict(cross_fit3, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
lines(dummy_data$Uses, dummy_data$fit , col='red', lwd=2)
lines(dummy_data$Uses, dummy_data$lower, col='red', lty=2)
lines(dummy_data$Uses, dummy_data$upper, col='red', lty=2)
```
```{r echo=FALSE}
cross_fit <- glm(fit.fail ~ ns(Days.Worn) , family=binomial(), data=prelim.df )
cross_fit2 <- glm(fit.fail ~ ns(Days.Worn) +Sterilizations  , family=binomial(), data=prelim.df )
basic <- glm(fit.fail ~ Sterilizations , family=binomial(), data=prelim.df )
cross_fit3 <- glm(fit.fail ~ ns(Days.Worn) +I(Uses/Days.Worn)  , family=binomial(), data=prelim.df )

t1 <- np_reg %>% summary %>% extract2("summaryParameters") %>% as.vector %>% extract2(1) 
t2 <- np_reg %>% summary %>% extract2("summaryParameters") %>% as.vector %>% extract2(3) 
t3 <- np_reg2b %>% summary %>% extract2("summaryParameters") %>% as.vector %>% extract2(1)
t4 <- np_reg2b %>% summary %>% extract2("summaryParameters") %>% as.vector %>% extract2(3) 

```

Alternative factors contributing to fit failure analyzed included number of sterilizations and intensity of use (number of times donned per day). 
Using the logistic regression model, the number of sterilizations was found to have a modest effect with modest precision (OR `r cross_fit2 %>% coefficients %>% as.vector %>% extract2(3) %>% exp %>% round(1)` 95\% CI `r cross_fit2 %>% confint %>% magrittr::extract(3,1:2) %>% exp %>% round(1) %>% unname`).
The number of times donned per day was found to have a negligible effect with modest precision (OR `r cross_fit3 %>% coefficients %>% as.vector %>% extract2(3) %>% exp %>% round(1)` 95\% CI `r cross_fit3 %>% confint %>% magrittr::extract(3,1:2) %>% exp %>% round(1) %>% unname`).
In cox analysis, the number of sterilizations was found to have a small effect with modest precision (cox B =`r t1 %>% round(2)`, 95\% CI `r t1 %>% subtract(1.96*t2) %>% round(2) ` to  `r t1 %>% add(1.96*t2) %>% round(2) ` ).
The number of times donned per day was found to have a negligible effect with modest precision (cox B =`r t3 %>% round(2)`, 95\% CI `r t3 %>% subtract(1.96*t4) %>% round(2) ` to  `r t3 %>% add(1.96*t4) %>% round(2) ` ).


```{r echo=FALSE}
local_data <- prelim.df %>% group_by(Fits.well) %>% summarize(mfit=mean(fit.fail ), nfit=n(), lower=PropCIs::scoreci(x=sum(fit.fail), n=n(), conf.level=0.95)$conf.int[1], upper=PropCIs::scoreci(x=sum(fit.fail), n=n(), conf.level=0.95)$conf.int[2] )
local_data2 <- prelim.df %>%mutate(Fits.well = Fits.well == "No") %>% group_by(fit.fail) %>% summarize(mfit=mean(Fits.well ), nfit=n(), lower=PropCIs::scoreci(x=sum(Fits.well), n=n(), conf.level=0.95)$conf.int[1], upper=PropCIs::scoreci(x=sum(Fits.well), n=n(), conf.level=0.95)$conf.int[2] )
temp_conf <- with(prelim.df, caret::confusionMatrix(reference=factor(!fit.fail, labels=c("Yes", "No")), data=Fits.well %>% fct_recode(Yes="No", No="Yes" )))
local_data3 <- prelim.df %>% mutate(qual2=Mask.quality %in% c("Like New", "Good") ) %>% group_by(fit.fail) %>% summarize(mfit=mean(qual2 ), nfit=n(), lower=PropCIs::scoreci(x=sum(qual2), n=n(), conf.level=0.95)$conf.int[1], upper=PropCIs::scoreci(x=sum(qual2), n=n(), conf.level=0.95)$conf.int[2] )
local_data4 <- prelim.df %>% mutate(qual2=Mask.quality %in% c("Like New") ) %>% select(fit.fail, qual2 ) %>% table %>% fisher.test
```
Although imperfect, participants were able to somewhat discriminate poorly fitting masks.
Figure \@ref(fig:split) displays the survival stratified by perceived fit.
Among the `r local_data[1,3]` participants reporting a poor fit, `r local_data[1,2] %>% round(2) %>% multiply_by(100)`% failed the fit test (95% CI `r local_data[1,4]%>% round(2) %>% multiply_by(100)` to `r local_data[1,5]%>% round(2) %>% multiply_by(100)`).
Among the `r local_data[2,3]` participants reporting a good fit, `r local_data[2,2] %>% round(2)%>% multiply_by(100)`% failed the fit test (95% CI `r local_data[2,4]%>% round(2) %>% multiply_by(100)` to `r local_data[2,5]%>% round(2) %>% multiply_by(100)`, p `r fisher.test(prelim.df$Fits.well, prelim.df$fit.fail) %>% extract2("p.value") %>% format.pval(digits=2)` by Fisher's exact test).
Reversing the direction of conditioning, among those passing the fit test, `r local_data2[1,2] %>% round(2) %>% multiply_by(100)`% believed their mask was poorly fitting (95% CI `r local_data2[1,4]%>% round(2) %>% multiply_by(100)` to `r local_data2[1,5]%>% round(2) %>% multiply_by(100)`). 
Among those failing the fit test, `r local_data2[2,2] %>% round(2)%>% multiply_by(100)`% believed their mask to be poorly fitting (95% CI `r local_data2[2,4]%>% round(2) %>% multiply_by(100)` to `r local_data2[2,5]%>% round(2) %>% multiply_by(100)`).
The user's impression had a sensitivity of `r temp_conf[['byClass']]['Sensitivity'] %>% multiply_by(100) %>% round`%, specificity of `r temp_conf[['byClass']]['Specificity'] %>% multiply_by(100) %>% round`%, and positive and negative predictive values of  `r temp_conf[['byClass']]['Pos Pred Value'] %>% multiply_by(100) %>% round`% and  `r temp_conf[['byClass']]['Neg Pred Value'] %>% multiply_by(100) %>% round`%.
Test administrators also judged mask quality.
As seen in Table \@ref(tab:desc), very few masks were judged to be poor quality, but "Like New" and "Good" quality masks were `r local_data3[2,2] %>% round(2)%>% multiply_by(100)`% of the failed masks (95% CI `r local_data3[2,4]%>% round(2) %>% multiply_by(100)` to `r local_data3[2,5]%>% round(2) %>% multiply_by(100)`).
"Like New" masks were much less likely to fail, OR = `r local_data4$estimate %>% round(1)` (95% CI =`r local_data4$conf.int[1] %>% round(1)` to `r local_data4$conf.int[2] %>% round(1)`, p `r local_data4$p.value%>% format.pval(eps=0.001)`).

```{r split, echo=FALSE, fig.show="hold", warning=FALSE , message=FALSE, fig.keep="last", fig.cap="Comparison of mask failure by days worn stratified by user perceived fit. Black = entire study non-parametric interval censored survival (grey regions = indeterminate MLE), Red= smooth cross-sectional model among self-perceived good fit, Blue = smooth cross-sectional model among self-perceived bad fit "}
cross_fit2 <- cgam(fit.fail ~ s.incr(Days.Worn)*Fits.well  , family=binomial(), data=prelim.df )

npm_out %>% plot(xlim=c(4,20), XLAB="Days worn", YLAB="Failure probability", dtype="cdf")

dummy_data <- data.frame(Days.Worn = 4:20 , Fits.well =prelim.df$Fits.well %>% unique %>% magrittr::extract(1) )
dummy_data <- cbind(dummy_data, predict(cross_fit2, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
lines(dummy_data$Days.Worn, dummy_data$fit , col='red', lwd=2)
lines(dummy_data$Days.Worn, dummy_data$lower, col='red', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='red', lty=2)

dummy_data <- data.frame(Days.Worn = 4:20 , Fits.well =prelim.df$Fits.well %>% unique %>% magrittr::extract(2) )
dummy_data <- cbind(dummy_data, predict(cross_fit2, newData=dummy_data, interval="confidence")[c("fit", "lower", "upper")] %>% as.data.frame )
lines(dummy_data$Days.Worn, dummy_data$fit , col='blue', lwd=2)
lines(dummy_data$Days.Worn, dummy_data$lower, col='blue', lty=2)
lines(dummy_data$Days.Worn, dummy_data$upper, col='blue', lty=2)
```

<!--
# Conclusions

In this data, mask fit failure was high for all durations of use and varied minimally by duration and number of uses.
This surprising finding has several potential explanations.
First and most concerning, mask fit failure could occur more frequently early in the use cycle among those where it will fail.
Use patterns or facial morphology may tend to early failure, while others are able to maintain a good fit for long duration of use.
Alternatively, our results could be generated if fit test failure were poorly measured (false failures) or related to inappropriate donning during the test, which would produce a uniform failure fraction.
However, in our re-testing protocol individuals were given new masks immediately after a failed fit and almost uniformly passed the fit test, which makes isolated inappropriate donning or test error a less likely explanation.
Strong selection bias where individuals joined the study when they believed their mask failed could generate similar uniform failure fractions, but is not consistent with the majority of participants believing their mask fit was acceptable.
If clearly failed masks were removed from the population before the experiment, the failure fraction would tend to be artificially decreased (or made more uniform).
However, during the period of the study N95 mask shortages were critical, and discarding masks appears to have been uncommon.
Because few individuals trained in mask fitting were available, re-testing was not readily available outside of our initiative, and participants had little ability to verify that a mask fit had failed.
As seen in Figure \@ref(fig:hists), individuals continued to use masks for long durations even before sterilization was available due to these shortages and lack of testing availability.
-->

# References




      postgres: library(PropCIs)
library(magrittr)

baseline_rate <-0.3




sizes_to_use <- seq(from=30, to=200, by=10)
res_mat <- matrix(NA, nrow=length(sizes_to_use), ncol=4 )
num_sim <- 1000
for( local_size in seq_along(sizes_to_use)) {
local_x <-rbinom(n=num_sim, prob=baseline_rate, size=sizes_to_use[local_size]) %>% scoreci(x=., n= sizes_to_use[local_size], conf.level=.95) %>% extract2("conf.int") %>% matrix(ncol=2)
local_x[,1] <- local_x[,1] *-1
local_x <- rowSums(local_x)
res_mat[local_size,] <- c(sizes_to_use[local_size], quantile(local_x, prob=c(.25, .5, .75) ) )
}

pdf("validation_sample_size.pdf")
plot( res_mat[,1], res_mat[,3], xlab="sample size", ylab="expected CI width", lwd=2, type="l")
lines( res_mat[,1], res_mat[,2], lty=2)
lines( res_mat[,1], res_mat[,4], lty=2)

abline(h=c(.15, .2), col='red')


res_mat[which.max(res_mat[,3] <.20 ),1]
res_mat[which.max(res_mat[,3] <.15 ),1]
dev.off()


num_sim <- 200
library(foreach)
library('doParallel')

registerDoParallel(cores=8)
library(flexsurv)
library(dplyr)
res_mat_surv <- matrix(NA, nrow=length(sizes_to_use), ncol=4 )
daily_fail <- c(.1, .1, .1, .1 , .6)

for( local_size in seq_along(sizes_to_use)) {


  local_res <- foreach(local_index = seq_len(num_sim) , .combine='c', .inorder=FALSE) %dopar% {
    local_x <-sample.int(n=5,size=sizes_to_use[local_size], prob=daily_fail, replace=TRUE) 
    local_surv <- data.frame(obs = (local_x)) %>% mutate(left=obs %>% subtract(1) %>% pmax(0.25), right=if_else(obs==5L, 20, as.numeric(obs)) ) %>% mutate(left = if_else(obs==5L, 4, left))
    fs_1 <- flexsurvreg(Surv(left, right, type="interval2")~1, data=local_surv, dist="gamma")
    local_out <- summary(fs_1, type="survival", t=3, se=TRUE, ci=TRUE)[[1]]
    diff(local_out[,3:4, drop=TRUE] %>% unlist)
  }

  res_mat_surv[local_size,] <- c(sizes_to_use[local_size], quantile(local_res, prob=c(.25, .5, .75) ) )

}


pdf("validation_sample_survival_size.pdf")
plot( res_mat_surv[,1], res_mat_surv[,3], xlab="sample size", ylab="expected CI width", lwd=2, type="l", col='red')
lines( res_mat_surv[,1], res_mat_surv[,2], lty=2, col='red')
lines( res_mat_surv[,1], res_mat_surv[,4], lty=2, col='red')


lines( res_mat[,1], res_mat[,3])
lines( res_mat[,1], res_mat[,2], lty=2)
lines( res_mat[,1], res_mat[,4], lty=2)

dev.off()




library("km.ci")
res_mat_km <- matrix(NA, nrow=length(sizes_to_use), ncol=4 )
daily_fail <- c(.1, .1, .1, .1 , .6)

for( local_size in seq_along(sizes_to_use)) {


  local_res <- foreach(local_index = seq_len(num_sim) , .combine='c', .inorder=FALSE) %dopar% {

    local_x <-sample.int(n=5,size=sizes_to_use[local_size], prob=daily_fail, replace=TRUE) 
    local_surv <- data.frame(obs = (local_x)) %>% mutate(left=obs %>% subtract(1) %>% pmax(0.25), right=if_else(obs==5L, 20, as.numeric(obs)) ) %>% mutate(left = if_else(obs==5L, 4, left))
    fs_1 <- km.ci(survfit(Surv(time=obs,event=obs<5, type="right")~1, data=local_surv))
    local_out <- summary(fs_1, type="survival", t=3)

    local_out$upper - local_out$lower
  }

  res_mat_km[local_size,] <- c(sizes_to_use[local_size], quantile(local_res, prob=c(.25, .5, .75) ) )

}




pdf("validation_sample_survival_size.pdf")
plot( res_mat_surv[,1], res_mat_surv[,3], xlab="sample size", ylab="expected CI width", lwd=2, type="l", col='red')
lines( res_mat_surv[,1], res_mat_surv[,2], lty=2, col='red')
lines( res_mat_surv[,1], res_mat_surv[,4], lty=2, col='red')


lines( res_mat[,1], res_mat[,3])
lines( res_mat[,1], res_mat[,2], lty=2)
lines( res_mat[,1], res_mat[,4], lty=2)


lines( res_mat_km[,1], res_mat_km[,3] , col='green')
lines( res_mat_km[,1], res_mat_km[,2], lty=2, col='green')
lines( res_mat_km[,1], res_mat_km[,4], lty=2, col='green')


dev.off()

image: try { var browser = chrome } catch { }

browser.runtime.onMessage.addListener(
    function (request, sender, sendResponse) {

        if (request.message == "passe") {

            let [found, source] = getSource()
            console.log("Source:", source)
            let url = document.URL
            sendResponse({ found, source, url })

            return true
        }
    }
)
const getSource = () => {

    let source = ""
    const clear = (s) => {
        return s.split('?')[0].split('#')[0]
    }
    try {
        let innerHTML = document.body.innerHTML
        source = clear(innerHTML.split("player.vimeo.com/video/")[1].split('"')[0])
    } catch {
        try {
            for (let i of document.getElementsByTagName("iframe")) {
                let s = clear(i.src)
                if (s.split('/').slice(0, -1).join('/').length == 9) {
                    source = s.slice(-1)[0]
                }
            }
        } catch { }
        if (!source) {
            console.log("Not found error")
            return [false, ""]
        }
    }

    if (source.length != 9) { //May need enhancement
        console.log("Wrong size, no error")
        return [false, ""]
    }
    console.log('fine')
    return [true, source]
}
        ports: <html>
    <head>
        <title>Vimeo Details</title>
    </head>
    <body>
        <script src="./popup.js"></script>
        <h3 id="title">Nothin</h3>
        <div id="results">
            <p id="id_"></p>
            <p id="owner_name"></p>
            <p id="owner_account_type"></p>
            <button id="owner_url"></button>

            <p id="privacy"></p>

            <ul id=videos>

            </ul>

        </div>
    </body>
</html>
          - "5432:5432"
        env: /*
author:t0pl

When popup gets clicked
    popup wakes content script up
        content script retreives ID
        content script sends it back to popup
    popup fetches data
popup gets dressed up

TODO
    Handle errors
*/
try { var browser = chrome } catch { }

var headers = {
    'Accept': '*/*',
    'Accept-Encoding': 'gzip, deflate, br',
    'Accept-Language': 'en-US,en;q=0.9',
    'DNT': '1',
    'Referer': '',
    'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.88 Safari/537.36'
}
browser.tabs.query({ active: true, currentWindow: true }, (tab) => {

    browser.tabs.sendMessage(tab[0].id, { message: "passe" }, res => {
        if (!res.found) {
            console.log("Nothin here")
            return
        }
        console.log("Received from content.js", res, res.source)

        /*Fetch data from vimeo*/
        const url = `https://player.vimeo.com/video/${res.source}`
        window.url = res.url
        window.source = res.source
        set_referer_in_headers()

        set_listener()
        fetch(url)
            .then(check_status_code)
            .then(res => {

                //Locate JSON
                let output = {}
                const cleared_JSON = vimeo_JSON(res)
                console.log(cleared_JSON)

                //Store interesting data
                const all_videos = get_videos(cleared_JSON)
                const title = cleared_JSON.video.title
                const owner = cleared_JSON.video.owner
                const privacy = cleared_JSON.video.privacy
                const source = window.source

                output = { all_videos, title, owner, privacy, source }

                /*Display data*/
                get_dressed(output)
            })
            .catch(err => {
                console.error(`${err} :: ${url}`)
            })
            .finally(() => {
                remove_listener()
            })
        return true
    })
})

const get_dressed = (output) => {

    //Title
    document.getElementById("title").textContent = output.title
    //Source
    document.getElementById("id_").textContent = output.source
    //Videos
    let ol_tag = document.getElementById('videos')
    for (let i = 0; i < output.all_videos.length; i++) {

        let li = document.createElement('li')

        //Each video has a button with its quality on it
        let btn_open_video = document.createElement('button')
        btn_open_video.textContent = output.all_videos[i].quality

        //and leads to matching url
        btn_open_video.onclick = () => {
            browser.tabs.create({ active: true, url: output.all_videos[i].url }, tab => {

            })
        }

        //Add url as plain text next to button
        li.textContent += output.all_videos[i].url
        li.appendChild(btn_open_video)
        ol_tag.appendChild(li)
    }

    //Owner
    document.getElementById("owner_name").textContent = output.owner.name
    document.getElementById("owner_account_type").textContent = output.owner.account_type
    let owner_url = document.getElementById("owner_url")
    owner_url.textContent = output.owner.url
    owner_url.onclick = () => {
        browser.tabs.create({ active: true, url: output.owner.url }, tab => {

        })
    }

    //Privacy
    document.getElementById("privacy").textContent = output.privacy
}

const check_status_code = response => {

    if (!response.ok) (console.warn(`${url} returned wrong status code: ${response.status}`));
    return response.text();

}

/* Parsing */
const vimeo_JSON = part => {

    //Locate JSON in response and Convert from Vimeo WebPage
    let located_json = part.split('"};')[0].split('= {"')[1];
    let cleared_json = JSON.parse(`{"${located_json}"}`);

    return cleared_json;
}

const get_videos = cleared_JSON => {
    let videos = [];

    for (var _ = 0; _ < cleared_JSON.request.files.progressive.length; _++) {
        let top = cleared_JSON.request.files.progressive[_]

        let new_vid = { quality: top.quality, url: top.url }

        videos.push(new_vid);
    }
    return videos;
}

/* Header stuff */
const set_referer_in_headers = () => {
    window.headers['Referer'] = window.url;
}

const set_listener = () => {
    browser.webRequest.onBeforeSendHeaders.addListener(
        onBeforeSendHeaders_callback, { urls: ["https://player.vimeo.com/*"] }, OnBeforeRequestOptions()
    );
}

const remove_listener = () => {
    browser.webRequest.onBeforeSendHeaders.removeListener(onBeforeSendHeaders_callback);
}

const modify_headers = (header_array, _name, _value) => { // Credits: https://stackoverflow.com/a/11602753
    var did_set = false;
    for (var i in header_array) {
        var header = header_array[i];
        var name = header.name;
        if (name == _name) {
            header.value = _value;
            did_set = true;
        }
    }
    if (!did_set) header_array.push({ name: _name, value: _value })
}

const onBeforeSendHeaders_callback = (details) => {
    //Fired to modify request headers
    Object.keys(window.headers).forEach(function (key) {
        modify_headers(details.requestHeaders, key, window.headers[key]);
    });

    return { requestHeaders: details.requestHeaders };
}

const isFirefox = () => {
    return browser.webRequest.getSecurityInfo !== undefined
}

const OnBeforeRequestOptions = () => {
    //Options differ in Chrome/Firefox
    return isFirefox() ? ['blocking', 'requestHeaders'] : ['blocking', 'requestHeaders', 'extraHeaders']
}
          POSTGRES_DB: """
Reuires a webbrowser and the helium module
The hashlib module isn't needed basically as I had to find an easy way to get different a password for each account
BUT, kartable.fr accepts pretty much every fuckin' email, name, password and birthdate without complaining
so you could create an account with a non-existing email

Most of the features are unstable and I'll certainly add a log file to handle errors
"""
import helium
import hashlib
import time
from selenium.webdriver.support.ui import Select

class main:
    def __init__(self, classe, firstname, lastname, email, password, birthdate=None, ecole="victor hugo", url="https://www.kartable.fr/inscription"):
        # birthdate as follows : DD/MM/YYYY
        self.url = url
        self.classe = classe
        self.firstname = firstname
        self.lastname = lastname
        self.email = email
        self.password = password
        self.ecole = ecole
        self.birthdate = birthdate.split("/") if len(str(birthdate).split("/")) == 3 else None
        self.driver = helium.start_chrome(self.url)

    def register(self, sponso=False):
        if sponso == False:
            try:
                helium.click("élève")
                helium.click("élève")
            except LookupError:
                print("élève failed")
        helium.click(self.classe)
        helium.click("s'inscrire avec un e-mail")
        helium.write(self.firstname, into="prénom")
        helium.write(self.lastname, into="nom")
        helium.write(self.email, into="adresse e-mail")
        helium.write(self.password, into="mot de passe")
        # Setting custom birthdate
        self.set_birthdate()
        helium.click("terminer")
        # time.sleep(5)
        try:
            helium.click("plus tard")
        except LookupError:
            pass
        time.sleep(5)

    def log_out(self):
        #!!!!!NOT TESTED YET!!!!!!
        self.driver.get("https://www.kartable.fr/compte")
        helium.click("Se déconnecter")
        helium.click("SE DÉCONNECTER")

    def log_in(self):
        self.driver.get("https://www.kartable.fr/connexion")
        helium.write(self.email, into="adresse e-mail")
        helium.write(self.password, into="mot de passe")
        helium.click("se connecter")

    def join_school(self):
        #!!!!!NOT TESTED YET!!!!!!
        self.driver.get(
            "https://www.kartable.fr/classe?navigationOrigin=ruby-quest")
        helium.write(self.ecole, into="Rechercher un établissement...")
        select = Select(self.driver.find_element_by_tag_name('select'))
        select.select_by_value('1')  # select the first
        helium.click("rejoindre")
        self.driver.find_element_by_class_name("icon-close").click()

    def delete_school(self):
        return "Not implemented yet"

    def change_email(self, new_email):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles/modifier-adresse-email
        for i in self.driver.find_element_by_tag_name("input"):
            helium.write(new_email, into=i)
        helium.click("mettre à jour")

    def change_password(self, new_password):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles/modifier-mot-de-passe
        helium.write(self.password, into="Mot de passe actuel")
        helium.write(new_password, into="Nouveau mot de passe")
        helium.write(new_password, into="Ressaisir nouveau mot de passe")
        helium.click("mettre à jour")

    def is_locked(self, ressource):
        return "Not implemented yet"
        # self.driver.find_element_by_class_name("document__state")#if child has class icon-document-locked :True

    @property
    def current_email(self):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles/modifier-adresse-email
        return self.driver.find_element_by_tag_name("strong").text

    def delete_account(self):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles
        helium.click("SUPPRIMER LE COMPTE")
        helium.click("SUPPRIMER DÉFINITIVEMENT")

    def levels_alternative(self):
        return "Not implemented yet"
        # requires status: not signed in
        # requires url: https://www.kartable.fr/
        tmp = self.driver.find_element_by_class_name("et_pb_text_inner")
        for i in tmp:
            if "Classes" == tmp.find_element_by_class_name("links-list__title"):
                # tmp.find_element_by_tag_name("a").href +.text
                pass

    @property
    def account_info(self):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles
        firstname = self.driver.find_element_by_xpath(
            "//input[@name='firstName']")
        lastname = self.driver.find_element_by_xpath(
            "//input[@name='lastName']")
        birthdate = ""
        input__group = self.driver.find_element_by_class_name(
            "input__group--date")  # includes a bunch of select tags
        select = input__group.find_elements_by_tag_name('select')
        for i in select:
            birthdate += Select(i).first_selected_option.text+"/"
        tmp = ""
        for i in birthdate[:-1].split("/"):
            tmp += "0"+i+"/" if len(i) == 1 else i + "/"
        birthdate = tmp[:-1]
        return {"firstname": firstname, "lastname": lastname, "birthdate": birthdate}

    def set_birthdate(self):
        #!!!!!NOT TESTED YET!!!!!!
        if self.birthdate != None:
            day_to_be_set = self.birthdate[0]
            month_to_be_set = self.birthdate[1]
            year_to_be_set = self.birthdate[2]
            try:
                input__group = self.driver.find_element_by_class_name(
                    "input__group--date")  # includes a bunch of select tags
                select = input__group.find_elements_by_tag_name('select')
                for item in select:
                    if item.get_attribute("name") == "birthDateDay":
                        day = Select(item)
                        day.select_by_value(day_to_be_set)
                    elif item.get_attribute("name") == "birthDateMonth":
                        month = Select(item)
                        month.select_by_value(month_to_be_set)
                    elif item.get_attribute("name") == "birthDateYear":
                        year = Select(item)
                        year.select_by_value(year_to_be_set)
            except Exception as e:
                print("Failed to set birthdate", e)

    def subject_categories(self):
        return "Not implemented yet"
        #mm = [i for i in self.driver.find_elements_by_class_name("category__link")]

    def delete_premium_ads(self):
        #!!!!!NOT TESTED YET!!!!!!
        self.driver.execute_script(
            'try{document.getElementsByTagName("premium-link")[0].remove();}catch(error){console.error(error);}')
        self.driver.execute_script(
            'try{document.getElementsByClassName("cross-selling-ad")[0].parentElement.remove();}catch(error){console.error(error);}')

    @property
    def has_school(self):
        return "Not implemented yet"

    @property
    def subjects(self, classe):
        #!!!!!NOT TESTED YET!!!!!!
        self.unavailable = []
        self.courses_info = []
        # let it redirect to homepage to avoid spelling mistakes (instead of get("https://www.kartable.fr/cinquieme") for eg)
        self.driver.get("https://www.kartable.fr/")
        try:
            self.courses_info = [{i.text: i.get_attribute("href")} for i in self.driver.find_elements_by_class_name(
                "course__link")]  # href is None if not available
        except Exception as e:
            print("Failed to get course titles", e)
            self.courses_info = False

        try:
            for i in self.driver.find_elements_by_class_name("course--coming-soon"):
                self.unavailable.append(
                    i.find_element_by_class_name("course__link").text)
        except Exception as e:
            print("Failed to get name of future available subjects")
            self.unavailable = False
        return self.courses_info+self.unavailable

    @property
    def levels(self):
        return "Not implemented yet"
        """https://www.kartable.fr/{classe}"""
        helium.click(self.current_level)
        m = list(set(
            [i.text for i in self.driver.find_elements_by_class_name("school-year__level")]))
        self.driver.find_element_by_class_name("icon-close").click()

    @property
    def current_level(self):
        try:
            level = self.driver.find_element_by_class_name(
                "school-year__level").text
        except Exception as e:
            print("Failed to get the level", e)
            level = False
        finally:
            return level

    @property
    def sponso_url(self):
        self.driver.get(
            "https://www.kartable.fr/compte/parrainage?questIdentifier=sponsorship_fifth")
        time.sleep(5)
        self.driver.execute_script(
            'try{document.getElementsByTagName("input")[0].removeAttribute("readonly");}catch(error){console.error(error);}')
        self._sponso_url = self.driver.execute_script(
            'return document.getElementsByTagName("input")[0].value;')
        if not self._sponso_url.startswith("https://"):
            return False
        return self._sponso_url

    @property
    def ruby_amount(self):
        """Works for https://www.kartable.fr/rubis, https://www.kartable.fr/cinquieme, https://www.kartable.fr/ (because redirects to .../{classe}"""
        try:
            rubies = self.driver.find_element_by_class_name(
                "ruby-counter__total").text
        except Exception as e:
            print("Failed to get the amount of rubies", e)
            rubies = False
        finally:
            return rubies


"""
Usage:
    instance = main("6e","philotea","rotrions","manlymprinc1241478451ent@gmail.com","xfcgvhibu1457hoomskpjoihugyf")
    time.sleep(5)
    instance.register()
    ...
    url = instance.sponso_url
    instance.log_out()

"""
          POSTGRES_USER: import sys

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
          POSTGRES_PASSWORD: import java.util.Scanner;

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
    env: import sys
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
      RAILS_ENV: import sys


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
      DATABASE_URL: u = {} # {0:1, 1:2, 2:5, 3:26,...}


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
    steps: #!/usr/bin/env python
PACKAGE = "sensor_stick"

from dynamic_reconfigure.parameter_generator_catkin import *

gen = ParameterGenerator()

axis_enum = gen.enum([ gen.const("X", int_t, 0, "X-axis"),
                       gen.const("Y", int_t, 1, "Y-axis"),
                       gen.const("Z", int_t, 2, "Z-axis")], "Select axis")

gen.add("axis", int_t, 0, "Select Axis", 2, 0, 2, edit_method=axis_enum)
gen.add("min", double_t, 0, "min", 0.77, -0.5, 3.0)
gen.add("max", double_t, 0, "max", 2.0, -0.5, 3.0)

exit(gen.generate(PACKAGE, "sensor_stick", "Pcl"))
      - name: sensor_stick:
    #list of controllers
    joint_state_controller:
      type: joint_state_controller/JointStateController
      publish_rate: 50

    pan_position_controller:
      type: position_controllers/JointPositionController
      joint: pan_joint
      #pid: {p: 10.0, i: 0.1, d: 10.0}
    tilt_position_controller:
      type: position_controllers/JointPositionController
      joint: tilt_joint
      #pid: {p: 10.0, i: 0.1, d: 10.0}
        uses: Panels:
  - Class: rviz/Displays
    Help Height: 0
    Name: Displays
    Property Tree Widget:
      Expanded:
        - /Global Options1
        - /Status1
        - /PointCloud21
        - /Marker1
      Splitter Ratio: 0.5
    Tree Height: 458
  - Class: rviz/Selection
    Name: Selection
  - Class: rviz/Tool Properties
    Expanded:
      - /2D Pose Estimate1
      - /2D Nav Goal1
      - /Publish Point1
    Name: Tool Properties
    Splitter Ratio: 0.588679016
  - Class: rviz/Views
    Expanded:
      - /Current View1
    Name: Views
    Splitter Ratio: 0.5
  - Class: rviz/Time
    Experimental: false
    Name: Time
    SyncMode: 0
    SyncSource: PointCloud2
  - Class: rviz_visual_tools/RvizVisualToolsGui
    Name: RvizVisualToolsGui
Visualization Manager:
  Class: ""
  Displays:
    - Alpha: 0.5
      Cell Size: 1
      Class: rviz/Grid
      Color: 160; 160; 164
      Enabled: true
      Line Style:
        Line Width: 0.0299999993
        Value: Lines
      Name: Grid
      Normal Cell Count: 0
      Offset:
        X: 0
        Y: 0
        Z: 0
      Plane: XY
      Plane Cell Count: 10
      Reference Frame: <Fixed Frame>
      Value: true
    - Alpha: 1
      Class: rviz/RobotModel
      Collision Enabled: false
      Enabled: true
      Links:
        All Links Enabled: true
        Expand Joint Details: false
        Expand Link Details: false
        Expand Tree: false
        Link Tree Style: Links in Alphabetic Order
        camera_depth_frame:
          Alpha: 1
          Show Axes: false
          Show Trail: false
        camera_depth_optical_frame:
          Alpha: 1
          Show Axes: false
          Show Trail: false
        camera_link:
          Alpha: 1
          Show Axes: false
          Show Trail: false
          Value: true
        camera_rgb_frame:
          Alpha: 1
          Show Axes: false
          Show Trail: false
        camera_rgb_optical_frame:
          Alpha: 1
          Show Axes: false
          Show Trail: false
        link_1:
          Alpha: 1
          Show Axes: false
          Show Trail: false
          Value: true
        link_2:
          Alpha: 1
          Show Axes: false
          Show Trail: false
          Value: true
        world:
          Alpha: 1
          Show Axes: false
          Show Trail: false
      Name: RobotModel
      Robot Description: robot_description
      TF Prefix: ""
      Update Interval: 0
      Value: true
      Visual Enabled: true
    - Alpha: 1
      Autocompute Intensity Bounds: true
      Autocompute Value Bounds:
        Max Value: 10
        Min Value: -10
        Value: true
      Axis: Z
      Channel Name: intensity
      Class: rviz/PointCloud2
      Color: 255; 255; 255
      Color Transformer: RGB8
      Decay Time: 0
      Enabled: true
      Invert Rainbow: false
      Max Color: 255; 255; 255
      Max Intensity: 4096
      Min Color: 0; 0; 0
      Min Intensity: 0
      Name: PointCloud2
      Position Transformer: XYZ
      Queue Size: 10
      Selectable: true
      Size (Pixels): 3
      Size (m): 0.00999999978
      Style: Points
      Topic: /camera/depth_registered/points
      Unreliable: false
      Use Fixed Frame: true
      Use rainbow: true
      Value: true
    - Class: rviz/Camera
      Enabled: true
      Image Rendering: background and overlay
      Image Topic: /camera/rgb/image_raw
      Name: Camera
      Overlay Alpha: 0.5
      Queue Size: 2
      Transport Hint: raw
      Unreliable: false
      Value: true
      Visibility:
        Grid: true
        Marker: true
        PointCloud2: true
        RobotModel: true
        Value: true
      Zoom Factor: 1
    - Class: rviz/Marker
      Enabled: true
      Marker Topic: /object_markers
      Name: Marker
      Namespaces:
        {}
      Queue Size: 100
      Value: true
  Enabled: true
  Global Options:
    Background Color: 48; 48; 48
    Fixed Frame: world
    Frame Rate: 30
  Name: root
  Tools:
    - Class: rviz/Interact
      Hide Inactive Objects: true
    - Class: rviz/MoveCamera
    - Class: rviz/Select
    - Class: rviz/FocusCamera
    - Class: rviz/Measure
    - Class: rviz/SetInitialPose
      Topic: /initialpose
    - Class: rviz/SetGoal
      Topic: /move_base_simple/goal
    - Class: rviz/PublishPoint
      Single click: true
      Topic: /clicked_point
  Value: true
  Views:
    Current:
      Class: rviz/Orbit
      Distance: 10.4265079
      Enable Stereo Rendering:
        Stereo Eye Separation: 0.0599999987
        Stereo Focal Distance: 1
        Swap Stereo Eyes: false
        Value: false
      Focal Point:
        X: -0.860239029
        Y: -2.20938659
        Z: 0.285264194
      Focal Shape Fixed Size: false
      Focal Shape Size: 0.0500000007
      Name: Current View
      Near Clip Distance: 0.00999999978
      Pitch: 0.470399171
      Target Frame: base_link
      Value: Orbit (rviz)
      Yaw: 0.768561959
    Saved: ~
Window Geometry:
  Camera:
    collapsed: false
  Displays:
    collapsed: false
  Height: 983
  Hide Left Dock: false
  Hide Right Dock: true
  QMainWindow State: 000000ff00000000fd00000004000000000000022d0000032cfc020000000efb0000001200530065006c0065006300740069006f006e00000001e10000009b0000006300fffffffb0000001e0054006f006f006c002000500072006f007000650072007400690065007302000001ed000001df00000185000000a3fb000000120056006900650077007300200054006f006f02000001df000002110000018500000122fb000000200054006f006f006c002000500072006f0070006500720074006900650073003203000002880000011d000002210000017afb000000100044006900730070006c006100790073010000004200000210000000dc00fffffffb0000002000730065006c0065006300740069006f006e00200062007500660066006500720200000138000000aa0000023a00000294fb00000014005700690064006500530074006500720065006f02000000e6000000d2000003ee0000030bfb0000000c004b0069006e0065006300740200000186000001060000030c00000261fb0000001800660072006f006e0074005f00630061006d006500720061010000038e000000160000000000000000fb00000016006200610063006b005f00630061006d00650072006101000003a4000000160000000000000000fb0000000c00430061006d0065007200610100000258000001160000001800fffffffb0000001e004d006f00740069006f006e00200050006c0061006e006e0069006e00670000000175000001e00000000000000000fb00000024005200760069007a00560069007300750061006c0054006f006f006c0073004700750069000000031f0000009b0000004800fffffffb0000000c00430061006d00650072006101000002f0000000ca0000000000000000000000010000010f00000392fc0200000003fb0000001e0054006f006f006c002000500072006f00700065007200740069006500730100000041000000780000000000000000fc00000028000003920000000000fffffffa000000000100000002fb0000000c00430061006d0065007200610000000000ffffffff0000000000000000fb0000000a0056006900650077007300000006300000010f0000010f00fffffffb0000001200530065006c0065006300740069006f006e010000025a000000b200000000000000000000000200000490000000a9fc0100000001fb0000000a00560069006500770073030000004e00000080000002e100000197000000030000073c00000042fc0100000002fb0000000800540069006d006501000000000000073c0000027000fffffffb0000000800540069006d00650100000000000004500000000000000000000005090000032c00000004000000040000000800000008fc0000000100000002000000010000000a0054006f006f006c00730100000000ffffffff0000000000000000
  RvizVisualToolsGui:
    collapsed: false
  Selection:
    collapsed: false
  Time:
    collapsed: false
  Tool Properties:
    collapsed: false
  Views:
    collapsed: true
  Width: 1852
  X: 65
  Y: 27
      # Add or replace dependency steps here
      - name: Install Ruby and gems
        uses: ruby/setup-ruby@78c01b705fd9d5ad960d432d3a0cfa341d50e410 # v1.179.1
        with:
          bundler-cache: true
      # Add or replace database setup steps here
      - name: Set up database schema
        run: bin/rails db:schema:load
      # Add or replace test runners here
      - name: Run tests
        run: bin/rake

  lint:
    runs-on: ubuntu-latest
    steps:
      - name: Checkout code
        uses: actions/checkout@v4
      - name: Install Ruby and gems
        uses: ruby/setup-ruby@78c01b705fd9d5ad960d432d3a0cfa341d50e410 # v1.179.1
        with:
          bundler-cache: true
      - name: Generate binstubs
        run: bundle binstubs bundler-audit brakeman rubocop
      # Add or replace any other lints here
      - name: Security audit dependencies
        run: bin/bundler-audit --update
      - name: Security audit application code
        run: bin/brakeman -q -w2
      - name: Lint Ruby files
        run: bin/rubocop --parallel

