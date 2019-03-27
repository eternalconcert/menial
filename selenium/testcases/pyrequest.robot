*** Settings ***
Resource           common.robot
Suite Setup        Open browser to index page
Suite Teardown     Close Browser
Test Teardown      Go to    ${INDEX}

*** Variables ***
${INDEX}          http://python.localhost:9999

*** Test Cases ***
Open index
    Given an open index page
    Then index page should be open

Open sub page
    Given an sub page
    Then sub page should be open

Show get params
    Given open page to reply request param
    Then param name should be part of the page title

URL dispatcher
    Given an url with a variable ID     23
    Then the ID should be part of the title     23
    And an url with a variable ID     5
    Then the ID should be part of the title     5

*** Keywords ***
Index page should be open
    Page title should be    Python test page: index

Sub page should be open
    Page title should be    Python test page: sub

Page title should be
    [Arguments]     ${title}
    Title should be     ${title}

Param name should be part of the page title
    Page title should be    Python test page: Christian

The ID should be part of the title
    [Arguments]     ${ID}
    Page title should be    Item to edit: ${ID}

An open index page
    Go to       ${INDEX}

An sub page
    Go to       ${INDEX}/sub/

Open page to reply request param
    Go to       ${INDEX}/params/?name=Christian

An url with a variable ID
    [Arguments]     ${ID}
    Go to       ${INDEX}/items/${ID}/edit/

Open browser to index page
    Open browser    ${INDEX}    ${BROWSER}
