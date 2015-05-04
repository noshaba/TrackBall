% -----------------------------------------------------------------------
% isec.cls
% Based on pi1-muster.tex and defs.tex used before 2003 for
% PI 1 in Bremen.
% Adapted by Thomas Röfer and Malte Kuhn (uni@monkz.de)

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{ezb}
\LoadClass[10pt,a4paper,twoside]{article}
\usepackage[ngerman]{babel}
\usepackage[utf8]{inputenc}
\usepackage[T1]{fontenc}
\usepackage{graphicx}
\usepackage{moreverb}
\usepackage{xcolor}
\usepackage{fancyhdr}
\usepackage{listings}
\usepackage{hyperref}

% -----------------------------------------------------------------------
% Page layout

% enlarge page
\setlength{\textwidth}{16cm}
\setlength{\topmargin}{-1cm}
\setlength{\evensidemargin}{0cm}
\setlength{\oddsidemargin}{0cm}
\setlength{\textheight}{24cm}

% skip between paragraphs
\setlength{\parskip}{1ex}
% ... and no indentation at start of a new paragraph
\setlength{\parindent}{0ex}

\newcommand{\thissemester}{SoSe 2015}
\newcommand{\thistitle}{Echtzeitbildverarbeitung}

\pagestyle{fancy}
\thispagestyle{empty}

%Layout for lstlistings
\lstloadlanguages{java} % Java sprache laden, notwendig wegen option 'savemem'
\lstset{language=java}
\lstset{numbers=left, numberstyle=\tiny, numbersep=5pt}
\lstset{literate=%
{Ö}{{\"O}}1
{Ä}{{\"A}}1
{Ü}{{\"U}}1
{ß}{{\ss}}1
{ü}{{\"u}}1
{ä}{{\"a}}1
{ö}{{\"o}}1
}
\lstset{basicstyle=\ttfamily\scriptsize}
% \lstset{stringstyle=\ttfamily}
% \lstset{commentstyle=\ttfamily}
\lstset{showspaces=false}
\lstset{showtabs=false}
\lstset{showstringspaces=false}
%\lstset{keywordstyle=\bfseries}
\lstset{tabsize=2}
\lstset{frameround=ffff}
\lstset{extendedchars=true}
\lstset{postbreak=\space, breakindent=5pt, breaklines}
\definecolor{javared}{rgb}{0.6,0,0} % for strings
\definecolor{javagreen}{rgb}{0.25,0.5,0.35} % comments
\definecolor{javapurple}{rgb}{0.5,0,0.35} % keywords
\definecolor{javadocblue}{rgb}{0.25,0.35,0.75} % javadocblue
\lstset{
keywordstyle=\color{javapurple}\bfseries,
stringstyle=\color{javared},
commentstyle=\color{javagreen},
morecomment=[s][\color{javadocblue}]{/**}{*/}
}

% -----------------------------------------------------------------------
% Main macros

\renewcommand{\maketitle}[8]
          {\@maketitle{#1}{#2}{#3}{#4}{#5}{#6}{#7}{#8}{Übungsblatt}}
\renewcommand\thesection{Aufgabe \arabic{section}:}
\newcommand\skipsection{\stepcounter{section}}

% -----------------------------------------------------------------------
% Misc macros

\newcommand{\grenzop}[3]{\mathop{#1}\limits^{#2}_{#3}}
\newcommand{\langrechtspfeil}[2]{\grenzop{\longrightarrow}{#1}{#2}}
\newcommand{\bob}[0]{\hspace*{\fill}$\Box$}

% -----------------------------------------------------------------------

\renewcommand{\@maketitle}[9]
{
    \begin{tabular}[t]{lr}
      \begin{tabular}[t]{l}
        \begin{tabular}[t]{l@{\hskip 10mm}l}
            \thistitle & \thissemester \hspace{4ex} \\
            Prof. Dr.: {#3} \\
        \end{tabular} \vspace{5mm} \\
        {\Huge\sf {#9} {#1} } \\[1.5ex]
        {\Large } \\[1ex]
        {\Large Abgabe am: {#2}} \\[1ex]
      \end{tabular}
      &
      \begin{tabular}[t]{l}
          {#5}\\
          {#6}\\
          {#7}\\
          {#8}\\
      \end{tabular} \\[2ex]
    \end{tabular}\\
    \rule{\linewidth}{1pt}

% -----------------------------------------------------------------------
% Create headings (Thanks to Elmar Loos)

    \fancyhead{} %clear all fields
    \fancyhead[LO,RE]{Echtzeitbildverarbeitung, \thissemester}
    %\fancyhead[RO,LE]{Prof: {#3}, Gruppe: {#4}, {#5}, {#6}, {#7}}
    \fancyfoot{} %clear all fields
    \fancyfoot[RO,LE]{\thepage}
}

\endinput
