*** Settings ***
Library            String
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

Post request data
    [Documentation]     Post a form and validate the received data
    Given open page to post form data
    And submit credentials   Christian       test123
    Then page title should contain post data    Christian       test123

Method dispatching POST
    [Documentation]     Post a form and validate the request method
    Given open page to post form data
    And submit credentials   Christian       test123
    Then page should contain method name     POST

Method dispatching GET
    [Documentation]     Open an URL which can process POST data or show a page
    Go to       ${INDEX}/process_post/
    Then page should contain method name     GET

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
    And an url with a variable name and an variable ID       benjamin       5
    Then both variable parts should be part of the title     benjamin       5

Dispatch URL with multiple variables with type names
    [Documentation]      This testcase has been written before implementing the feature
    Given an url with a variable name and an variable ID with type name     christian      23   19.0
    Then All variable parts should be part of the title                     christian      23   19.0
    And an url with a variable name and an variable ID with type name       benjamin       5    7.0
    Then All variable parts should be part of the title                     benjamin       5    7.0

Dispatcher should match without trailing /
    Given an URL without a trailing /
    Then the dispatcher should match the corresponding URL anyway

Redirect with fixed location
    Given an URL which redirect to another location
    Then the request should be redirect to the new location with a fix url

Redirect with variable location
    Given an URL which redirect to another location with a variable part
    Then the request should be redirect to the new location with the variable url       christian

Session
    ${Name}                                                 Generate Random String
    Given an URL which stores a variable to the session     ${Name}
    Then the variable should be present on another page     ${Name}

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

An url with a variable name and an variable ID with type name
    [Arguments]     ${Name}   ${ID}     ${Rate}
    Go to       ${INDEX}/suppliers/${Name}/offers/${ID}/taxrate/${Rate}/

All variable parts should be part of the title
    [Arguments]     ${Name}   ${ID}     ${Rate}
    Page title should be    Supplier: ${Name} - Offers: ${ID} (Tax rate: ${Rate})

Both variable parts should be part of the title
    [Arguments]     ${Name}   ${ID}
    Page title should be    Customer: ${Name} - Order: ${ID}

An URL without a trailing /
    Go to       ${INDEX}/no/trailing/slash

The dispatcher should match the corresponding URL anyway
    Page title should be    Matched it anyway

An URL which redirect to another location
    Go to       ${INDEX}/redirect/

The request should be redirect to the new location with a fix url
    Page title should be    Redirected from another location

An URL which redirect to another location with a variable part
    Go to       ${INDEX}/redirect/with/variable/part/

The request should be redirect to the new location with the variable url
    [Arguments]     ${Variable Part}
    Page title should be    Redirected from another location: ${Variable Part}

An URL which stores a variable to the session
    [Arguments]     ${Name}
    Go to           ${INDEX}/session/write/${Name}/

The variable should be present on another page
    [Arguments]             ${Name}
    Go to                   ${INDEX}/session/read/
    Page title should be    ${Name}

Open page to post form data
    Go to       ${INDEX}/post_form/

Page title should contain post data
    [Arguments]     ${Name}     ${Password}
    Page title should be    Name: ${Name}, Password: ${Password}

Page should contain method name
    [Arguments]     ${Method}
    Headline should be      ${Method}

Submit credentials
    [Arguments]     ${Name}     ${Password}
    Input text      xpath=//form/input[@name="name"]        ${Name}
    Input text      xpath=//form/input[@name="password"]    ${Password}
    Click button    submit

Open browser to index page
    Open browser    ${INDEX}    ${BROWSER}
