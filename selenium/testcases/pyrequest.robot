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

Show single get param
    [Documentation]     Show a single get parameter as part of the page title to make sure, get params are passed to python
    Given open page to reply request param
    Then param name should be part of the page title

Show multiple get params
    [Documentation]     Show multiple get parameters as part of the page title to make sure, multiple get params are passed to python
    Given open page to reply multiple request params
    Then param name and param id should be part of the page title

Dispatch URL with single variable
    [Documentation]     This testcase has been written before implementing the feature
    Given an url with a variable ID     23
    Then the ID should be part of the title     23
    And an url with a variable ID     5
    Then the ID should be part of the title     5

Dispatch URL with multiple variables
    [Documentation]      This testcase has been written before implementing the feature
    Given an url with a variable name and an variable ID     christian      23
    Then both variable parts should be part of the title     christian      23
    And an an url with a variable name and an variable ID    benjamin       5
    Then both variable parts should be part of the title     benjamin       5

*** Keywords ***
Index page should be open
    Page title should be    Python test page: index

Sub page should be open
    Page title should be    Python test page: sub

Page title should be
    [Arguments]     ${title}
    Title should be     ${title}

Open page to reply request param
    Go to       ${INDEX}/params/?name=Christian

Param name should be part of the page title
    Page title should be    Python test page: Christian

Open page to reply multiple request params
    Go to       ${INDEX}/params/?name=Christian&id=23

Param name and param id should be part of the page title
    Page title should be    Python test page: Name: Christian, ID: 23

An open index page
    Go to       ${INDEX}

An sub page
    Go to       ${INDEX}/sub/

An url with a variable ID
    [Arguments]     ${ID}
    Go to       ${INDEX}/items/${ID}/edit/

The ID should be part of the title
    [Arguments]     ${ID}
    Page title should be    Item to edit: ${ID}

An url with a variable name and an variable ID
    [Arguments]     ${Name}   ${ID}
    Go to       ${INDEX}/customers/${Name}/orders/${ID}/

Both variable parts should be part of the title
    [Arguments]     ${Name}   ${ID}
    Page title should be    Customer: ${Name} - Order: ${ID}

Open browser to index page
    Open browser    ${INDEX}    ${BROWSER}
