*** Settings ***
Resource           common.robot
Suite Setup        Open browser to dirlisting root
Suite Teardown     Close Browser
Test Teardown      Go to    ${INDEX}

*** Variables ***
${INDEX}          http://files.localhost:8000

*** Test Cases ***
Open root
    Index dirlisting root page should be open

Dirlisting content parent directory link
    First list item should be        Parent directory..

Dirlisting content second list item
    Second list item should be      static

Descending into directory
    Click link      styles/
    Second list item should be      main.css

Ascending into parent directory
    Click link      styles/
    Click link      ..
    Index dirlisting root page should be open

*** Keywords ***
Open browser to dirlisting root
    Open browser    ${INDEX}    ${BROWSER}

Index dirlisting root page should be open
    Title should be     Content of /

Nth list item should be
    [Arguments]    ${ListIndex}     ${Expected}
    ${Current}      get text    xpath=//ul/li[${ListIndex}]
    Should be equal      ${Current}      ${Expected}

First list item should be
    [Arguments]     ${Expected}
    Nth list item should be     1   ${Expected}

Second list item should be
    [Arguments]     ${Expected}
    Nth list item should be     2   ${Expected}
