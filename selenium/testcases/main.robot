*** Settings ***
Resource           common.robot
Suite Setup        Open browser to index page
Suite Teardown     Close Browser
Test Teardown      Go to    ${INDEX}

*** Variables ***
${INDEX}          http://localhost:8000

*** Test Cases ***
Open index
    Index page should be open

Open downloads
    Click link      download.html
    Headline should be      Download

Open help
    Click link      help.html
    Headline should be      Help

Open documentation
    Click link      documentation.html
    Headline should be      Documentation

Open license
    Click link      license.html
    Headline should be      menial

Open about
    Click link      /
    Headline should be      About

*** Keywords ***
Index page should be open
    Title should be     menial: Always at your service!

Open browser to index page
    Open browser    ${INDEX}    ${BROWSER}
