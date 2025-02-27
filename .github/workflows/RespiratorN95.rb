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

image: .ad-author, .ad-city {
    font-style: italic;
    font-weight: bold;
    float: left;
    margin-right: 30px;
} 
ports: .nofloat {
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

        env: .category-select {
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
          POSTGRES_DB: .form-inline .form-control {
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
 
          POSTGRES_USER: .navbar-default {
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
          POSTGRES_PASSWORD: .form-buttons {
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
    env:
      RAILS_ENV: test
      DATABASE_URL: "postgres://rails:password@localhost:5432/rails_test"
    steps:
      - name: Checkout code
        uses: actions/checkout@v4
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


