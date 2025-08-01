// Copyright 2019 - University of Strathclyde, King's College London and Schlumberger Ltd
// This source code is licensed under the BSD license found in the LICENSE file in the root directory of this source tree.

#include "LaTeXSupport.h"
#include "ptree.h"

namespace VAL {

extern analysis an_analysis;

LaTeXSupport latex;

void displayFailedLaTeXList(vector< string > &vs) {
    string s;
    for (vector< string >::const_iterator i = vs.begin(); i != vs.end();) {
        s = *i;
        replaceSubStrings(s, "/", "/\\-");
        latexString(s);
        *report << "\\begin{minipage}[t]{13cm} " << s << " \\end{minipage}";
        if (++i != vs.end()) *report << "\\\\";
        *report << "\n";
    };
};

void LaTeXSupport::LaTeXBuildGraph(ActiveCtsEffects *ace, const State *s) {
    for (map< const FuncExp *, ActiveFE * >::const_iterator fe =
                ace->activeFEs.begin();
            fe != ace->activeFEs.end(); ++fe) {
        FEGraph *feg = s->getValidator()->getGraph(fe->first);
        double intEndTime = s->getTime();  // time at start of interval

        feg->happenings.insert(intEndTime - ace->localUpdateTime);
        feg->happenings.insert(intEndTime);

        // setup initial value if nec, must be already to update cts so can assume
        // defined in prob file here
        if (feg->initialTime == -1) {
            feg->initialTime = 0;
            feg->initialValue = fe->first->evaluate(s);
        };

        if ((fe->second)->isLinear()) {
            feg->points[intEndTime - ace->localUpdateTime] =
                fe->second->evaluate(0);
            feg->points[intEndTime] = fe->second->evaluate(ace->localUpdateTime);
        } else {
            // number dividing is the number of points across whole interval if all
            // cts
            double stepLength = s->getValidator()->getMaxTime() / NoGraphPoints;

            if (const NumericalSolution *ns =
                        dynamic_cast< const NumericalSolution * >(fe->second->ctsFtn)) {
                map< double, CoScalar > thePoints = ns->getPoints();

                map< double, CoScalar >::const_iterator j = thePoints.begin();

                feg->points[intEndTime - ace->localUpdateTime + j->first] = j->second;
                CoScalar lastPoint = j->first;
                ++j;

                for (map< double, CoScalar >::const_iterator j = thePoints.begin();
                        j != thePoints.end(); ++j) {
                    if (j->first > ace->localUpdateTime) break;
                    if (j->first - lastPoint >= stepLength) {
                        feg->points[intEndTime - ace->localUpdateTime + j->first] =
                            j->second;
                        lastPoint = j->first;
                    };
                };
            } else {
                for (double t = 0; t < ace->localUpdateTime; t += stepLength) {
                    feg->points[intEndTime - ace->localUpdateTime + t] =
                        fe->second->evaluate(t);
                };
            };
            feg->points[intEndTime] = fe->second->evaluate(ace->localUpdateTime);
        };
    };
};

void LaTeXSupport::LaTeXHeader() {
    *report
            << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
            << "%                                              %\n"
            << "%  Plan Validation Report Generated by VAL     %\n"
            << "%                                              %\n"
            << "%  Strathclyde Planning Group                  %\n"
            << "%                                              %\n"
            << "%                                              %\n"
            << "%  Comments on Report to                       %\n"
            << "%              richard.howey@cis.strath.ac.uk  %\n"
            << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
            << "\\documentclass[a4paper,12pt]{article}\n"
            << "\\usepackage{rotating,color}\n"
            << "\\newcommand{\\headingtimeaction}{{\\bf Time} \\qquad \\= {\\bf "
       "Action}\\\\[0.8ex]}\n"
            << "\\newcommand{\\headingtimehappening}{{\\bf Time} \\qquad \\= "
       "{\\bf Happening}\\\\[0.8ex]}\n"
            << "\\newcommand{\\headingtimedetails}{{\\bf Time} \\qquad \\= {\\bf "
       "Details}\\\\[0.8ex]}\n"
            << "\\newcommand{\\atime}[1]{{\\bf #1:}}\n"
            << "\\newcommand{\\action}[1]{{\\sf #1}}\n"
            << "\\newcommand{\\exprn}[1]{{\\sf #1}}\n"
            << "\\newcommand{\\fexprn}[1]{{\\small {\\bf #1}}}\n"
            << "\\newcommand{\\condeffmon}[1]{#1 {\\it - conditional effect "
       "monitor}}\n"
            << "\\newcommand{\\updatectspne}{{\\it Update of changing Primitive "
       "Numerical Expressions}}\n"
            << "\\newcommand{\\actionstart}[1]{#1 {\\it - start}}\n"
            << "\\newcommand{\\actionend}[1]{#1 {\\it - end}}\n"
            << "\\newcommand{\\actioninv}[1]{{\\it Invariant for } #1}\n"
            << "\\newcommand{\\checkhappening}{Checking Happening... }\n"
            << "\\newcommand{\\eventtriggered}{{\\bf Event triggered!}}\n"
            << "\\newcommand{\\listrow}[1]{\\begin{minipage}[t]{11.5cm} #1 "
       "\\end{minipage}}\n"
            << "\\newcommand{\\listrowg}[1]{\\begin{minipage}[t]{10cm} #1 "
       "\\end{minipage}}\n"
            << "\\newcommand{\\happeningOK}{...OK!}\n"
            << "\\newcommand{\\notOK}{...NOT OK!}\n"
            << "\\newcommand{\\aprocessactivated}[1]{\\listrow{{\\it Activated "
       "process #1} }}\n"
            << "\\newcommand{\\aprocessunactivated}[1]{\\listrow{{\\it "
       "Unactivated process #1} }}\n"
            << "\\newcommand{\\aeventtriggered}[1]{\\listrow{{\\it Triggered "
       "event #1} }}\n"
            << "\\newcommand{\\assignment}[3]{\\listrow{Updating \\fexprn{#1} "
       "(#2) by #3 assignment.}}\n"
            << "\\newcommand{\\assignmentcts}[3]{\\listrow{Updating \\fexprn{#1} "
       "(#2) by #3 for continuous update.}}\n"
            << "\\newcommand{\\increase}[3]{\\listrow{Increasing \\fexprn{#1} "
       "(#2) by #3.}}\n"
            << "\\newcommand{\\decrease}[3]{\\listrow{Decreasing \\fexprn{#1} "
       "(#2) by #3.}}\n"
            << "\\newcommand{\\scaleup}[3]{\\listrow{Scaling up \\fexprn{#1} "
       "(#2) by a factor of #3.}}\n"
            << "\\newcommand{\\scaledown}[3]{\\listrow{Scaling down \\fexprn{#1} "
       "(#2) by a factor of #3.}}\n"
            << "\\newcommand{\\function}[2]{\\listrow{\\fexprn{#1}$(t) = #2$}}\n"
            << "\\newcommand{\\adding}[1]{\\listrow{Adding \\exprn{#1} }}\n"
            << "\\newcommand{\\deleting}[1]{\\listrow{Deleting \\exprn{#1} }}\n"
            << "\\newcommand{\\error}{...Error!\\\\}\n"
            << "\\newcommand{\\errorr}[1]{...Error! \\\\ \\> #1}\n"
            << "\\renewcommand{\\thefigure}{\\arabic{section}.\\arabic{figure}}"
            << "\\title{Plan Validation Report}\n"
            << "\\author{\\mbox{\\sc {\\sc Val}}}\n"
            << "\\begin{document}\n \\maketitle \n";
};

void LaTeXSupport::LaTeXDomainAndProblem() {
    string d = current_analysis->the_domain->name;
    latexString(d);
    string p = current_analysis->the_problem->name;
    latexString(p);
    *report << "\\section{Domain and Problem}\n";
    *report << "{\\bf Domain:} " << d << "\\\\\n";
    *report << "{\\bf Problem:} " << p << "\n";
};

void LaTeXSupport::LaTeXPlanReport(Validator *v, plan *the_plan) {
    *report << "\\subsection{Plan}\n";
    v->displayInitPlanLaTeX(the_plan);
    *report << "\\subsection{Plan To Validate}\n";
    v->displayPlan();

    *report << "\\subsection{Plan Validation}\n";
};

void LaTeXSupport::LaTeXGantt(Validator *v) {
    pddl_type *type;

    // setup Gantt chart objects to be tracked (coloured)
    for (vector< string >::iterator go = ganttObjectsAndTypes.begin();
            go != ganttObjectsAndTypes.end(); ++go) {
        type = current_analysis->pddl_type_tab.symbol_probe(*go);
        if (type != NULL) {
            for (typed_symbol_list< const_symbol >::const_iterator o =
                        current_analysis->the_problem->objects->begin();
                    o != current_analysis->the_problem->objects->end(); ++o) {
                if ((*o)->type == type) ganttObjects.push_back((*o)->getName());
            };
        } else {
            ganttObjects.push_back(*go);
        };
    };

    v->setSigObjs(ganttObjects);
    *report << "\\subsection{Gantt Chart}\n";
    v->drawLaTeXGantt(noGCPages, noGCPageRows);
};

void LaTeXSupport::LaTeXGraphs(Validator *v) {
    *report << "\\subsection{Primitive Numerical Expression Graphs}\n";

    *report << "\\setcounter{figure}{0}\n";
    v->displayLaTeXGraphs();
};

void LaTeXSupport::LaTeXPlanReportPrepare(char *pnm) {
    string planName(pnm);
    replaceSubStrings(planName, "/", "/\\-");
    latexString(planName);
    *report << "\\section{\\sloppy Plan: " << planName << "}\n";
};

void LaTeXSupport::LaTeXEnd() {
    *report << "\\end{document}\n";
};

};  // namespace VAL
