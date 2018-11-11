#define MULTI_LINE_STRING(a) #a

const char* websiteText = MULTI_LINE_STRING(
<!DOCTYPE HTML>
<HTML lang="en-US">
  <head>
    <meta charset='utf-8'>
    <meta http-equiv='X-UA-Compatible' content='IE=edge'>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <style>
        body {
            font-size: 3.5em;
        }
        h1 {
            font-size: 2em;
            text-align: center;
        }
        /*
        .toggle.ios, .toggle-on.ios, .toggle-off.ios { border-radius:20px;}
        .toggle.ios .toggle-handle { border-radius: 20px; }
        .slow .toggle-group { transition: left 0.7s; -webkit-transition: left 0.7s; }
        */
    </style>
    <link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css' integrity='sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u' crossorigin='anonymous'>
    <link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css' integrity='sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp' crossorigin='anonymous'>
    <link href='https://gitcdn.github.io/bootstrap-toggle/2.2.2/css/bootstrap-toggle.min.css' rel='stylesheet'>
  </head>
  <BODY onload='process()'>
    <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>
    <script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js' integrity='sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa' crossorigin='anonymous'></script>
    <script src='https://gitcdn.github.io/bootstrap-toggle/2.2.2/js/bootstrap-toggle.min.js'></script>
    <script src='https://use.fontawesome.com/5ff54f6697.js'></script>
    
    <div class='container-fluid'>
        <div class='row'>
            <div class='col-md-4 col-sm-1 col-xs-1'></div>
            <div class='col-md-4 col-sm-10 col-xs-10' id='appContainer'>
                <BR>
                <h1>GRINDER</h1>
                <BR>
                <div class='row'>
                    <h3>COFFEE MACHINE</h3>
                    <div class='col-md-9 col-sm-9 col-xs-9' id='rt'>
                        Runtime <A id='runtime'></A>
                    </div>
                    <div class='col-md-3 col-sm-3 col-xs-3'></div>
                </div>
                <div class='row'>
                    <div class='col-md-9 col-sm-9 col-xs-9' id='temperature'>
                        Temperature <A id='temp'></A>&#176;F
                    </div>
                    <BR>
                    <div class='col-md-3 col-sm-3 col-xs-3'></div>
                </div>

                <BR><BR>
                <div class='row'>
                    <div class='col-md-6 col-sm-8 col-xs-8' id='coffeeMachinePowerStatus'>
                        Coffee machine <A id='pinStatus'></A>
                    </div>
                    <div class='col-md-6 col-sm-3 col-xs-3' id='coffeeMachinePowerBtn'>
                        <input id='powerBtn1' disabled type='checkbox' data-onstyle='danger' data-toggle='toggle' data-style='ios slow' data-on='<i class=\"fa fa-power-off\"></i> On&nbsp;' data-off='<i class=\"fa fa-power-off\"></i> Off&nbsp;'></input>
                    </div>
                </div>

                <BR><BR>
                <div class='row'>
                    <div class='col-md-12 col-sm-12 col-xs-12 text-center'>
                        <A id='loading'></A>
                    </div>
                </div>
            </div>
            <div class='col-md-4 col-sm-1 col-xs-1'></div>
        </div>
    </div>
       
    <SCRIPT type='text/javascript'>   
      var PowerOn = false;
      var PowerBtn1On = false; 
      var xmlHttp=createXmlHttpObject();
      
      function createXmlHttpObject(){
        if(window.XMLHttpRequest){
          xmlHttp=new XMLHttpRequest();
        } else {
          xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');
        }
        return xmlHttp;
      }
    
      function process(){
        if(xmlHttp.readyState == 0 || xmlHttp.readyState == 4) {
          xmlHttp.open('PUT','xml',true);
          xmlHttp.onreadystatechange=handleServerResponse;
          xmlHttp.send(null);
        }
        setTimeout('process()',1000);
      }
    
      function handleServerResponse() {
        if(xmlHttp.readyState < 4) {
          
        } else if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {
          document.getElementById('loading').innerHTML = '';
          xmlResponse=xmlHttp.responseXML;
          xmldoc = xmlResponse.getElementsByTagName('temp');
          temp = xmlHttp.responseXML.getElementsByTagName('temp')[0].firstChild.nodeValue;
          runtime = xmlHttp.responseXML.getElementsByTagName('runtime')[0].firstChild.nodeValue;
          pinStatus = xmlHttp.responseXML.getElementsByTagName('pinStatus')[0].firstChild.nodeValue;
          document.getElementById('runtime').innerHTML=runtime+' seconds';
          document.getElementById('temp').innerHTML=temp;
          document.getElementById('pinStatus').innerHTML=pinStatus;
          if(pinStatus == 'on') { 
            PowerOn = true; 
          } else if (pinStatus == 'off') { 
            PowerOn = false; 
          }
        }
      }
    
      //used for post
      function togglePower(hideLoading) {
        if(((PowerOn == true) && (PowerBtn1On == false)) ||  ((PowerOn == false) && (PowerBtn1On == true))) { 
          document.getElementById('loading').innerHTML = 'Loading...';
          var http = new XMLHttpRequest();
          var url = 'togglePower';
          var params = 'togglePower=togglePower';
          http.open('POST','togglePower',true);
          http.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
          //http.onreadystatechange = function togglePower(){}
          http.send(params);
        }
        $('#powerBtn1').bootstrapToggle('enable'); 
      }
    
      $(document).ready(function() { 
        window.setTimeout(fixPowerBtns,3000); 
        $('#coffeeMachinePowerBtn').on('click', function() { if(PowerBtn1On === true){PowerBtn1On = false} else (PowerBtn1On = true); }); 
        $('#powerBtn1').change(function() { 
          $('#powerBtn1').bootstrapToggle('disable');
          togglePower(); 
        }); 
      }); 
    
      function fixPowerBtns() {
        $('#powerBtn1').bootstrapToggle('enable'); 
        if(PowerOn === true ) {
          PowerBtn1On = true; $('#powerBtn1').bootstrapToggle('on');
        } else { 
          PowerBtn1On = false; $('#powerBtn1').bootstrapToggle('off'); 
        } 
      }
    
      function hideLoading() {
        document.getElementById('loading').innerHTML = '';
      }   
    </SCRIPT>   
  </BODY>
</HTML>
);

void buildWebsite() {
  WebSite = websiteText;
}

void buildXML() {
  XML = "<?xml version='1.0'?>";
  XML += "<response>";
  XML += "<runtime>";
  XML += getRuntime();
  XML += "</runtime>";
  XML += "<temp>";
  XML += getTemperature();
  XML += "</temp>";
  XML += "<pinStatus>";
  XML += getRelayStatus();
  XML += "</pinStatus>";
  XML += "</response>";
}
