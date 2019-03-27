*** Settings ***
Documentation     Testcases for menial.
Library           SeleniumLibrary

*** Variables ***
${BROWSER}        Firefox

*** Keywords ***
Headline should be
    [Arguments]     ${Headline}
    ${Current}      get text    xpath=//h2
    Should be equal      ${Current}      ${Headline}

Click link [Arguments]
    Click link xpath=//a[@href=Arguments]
