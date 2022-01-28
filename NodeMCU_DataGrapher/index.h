const char MAIN_page[] PROGMEM = R"=====(
<!doctype html>
<html>

<head>
  <title>Data Graph | Stoper</title>
  <script src = "https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.7.3/Chart.min.js"></script>  
  <style>
  canvas{
    -moz-user-select: none;
    -webkit-user-select: none;
    -ms-user-select: none;
  }

  
  #dataTable {
    font-family: "Trebuchet MS", Arial, Helvetica, sans-serif;
    border-collapse: collapse;
    width: 100%;
  }

  #dataTable td, #dataTable th {
    border: 1px solid #ddd;
    padding: 8px;
  }

  #dataTable tr:nth-child(even){background-color: #f2f2f2;}
 
  #dataTable tr:hover {background-color: #ddd;}
  #dataTable th {
    padding-top: 12px;
    padding-bottom: 12px;
    text-align: left;
    background-color: #4CAF50;
    color: white;
  }
 
  </style>
</head>

<body>
    <div style="text-align:center;"><b>Accelometer</b><br>Mpu6050 on ESP8266</div>
    <div class="chart-container" position: relative; height:350px; width:100%">
        <canvas id="Chart" width="400" height="400"></canvas>
    </div>
<div>
  <table id="dataTable">
    <tr><th>Time</th><th></th><th>Mcu Temp (&deg;C)</th><th>Ax</th><th>Ay</th><th>Az</th><th>Gx</th><th>Gy</th><th>Gz</th></tr>
  </table>
</div>
<br>
<br>  

<script>
var ADCvalues = [];
var Tvalues = [];
var Axvalues = [];
var Ayvalues = [];
var Azvalues = [];
var Gxvalues = [];
var Gyvalues = [];
var Gzvalues = [];
var timeStamp = [];
function showGraph()
{
    var ctx = document.getElementById("Chart").getContext('2d');
    var Chart2 = new Chart(ctx, {
        type: 'line',
        data: {
            labels: timeStamp,  
            datasets: [{
                label: "Ax",
                fill: false, //Try with true
                backgroundColor: 'rgba( 100, 200, 100 , 1)', 
                borderColor: 'rgba( 100, 200, 100 , 1)',
                  pointRadius:0,
                  borderWidth:2,
                data: Axvalues,
                
                
            },{
                label: "Ay",
                fill: false,  //Try with true
                backgroundColor: 'rgba( 200, 100, 100 , 1)', 
                borderColor: 'rgba( 200, 100, 100 , 1)', 
                  pointRadius:0,
                  borderWidth:2,
                data: Ayvalues,
                
            },{
                label: "Az",
                fill: false,  //Try with true
                backgroundColor: 'rgba( 10, 150, 150 , 1)', //Dot marker color
                borderColor: 'rgba( 10, 150, 150 , 1)', //Graph Line Color
                  pointRadius:0,
                  borderWidth:2,
                data: Azvalues,
                
            }
            ],
        },
        tooltips:{
          mode:'nearest',
          intersect: false
          },
        options: {
          animation:{
                  duration:0
                  },
                  hover: {
            animationDuration: 0 // duration of animations when hovering an item
        },
            title: {
                    display: true,
                    text: "Color Codes"
                },
               
            maintainAspectRatio: false,
            elements: {
            line: {
                    tension: 0.2 //Smoothening (Curved) of data lines
                  
                }
            },
            scales: {
                    yAxes: [{
                      max: 0.1,
                      min: -0.1,
                        ticks: {
                            stepSize: 0.1
                        }
                    }]
                    
            },
        }
    });

}

//On Page load show graphs
window.onload = function() {
  console.log(new Date().toLocaleTimeString());
};

setInterval(function() {
  getData();
 
}, 5); //100 milliseconds update rate

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
     //Push the data in array
  var time = (new Date().toLocaleTimeString());
  var txt = this.responseText;
  var obj = JSON.parse(txt);
      ADCvalues.push(obj.ADC);
      Tvalues.push(obj.Temperature);
      Axvalues.push(obj.Ax);
      Ayvalues.push(obj.Ay);
      Azvalues.push(obj.Az);
      Gxvalues.push(obj.Gx);
      Gyvalues.push(obj.Gy);
      Gzvalues.push(obj.Gz);
      timeStamp.push(time);
      showGraph();
  
      //Update Graphs
  //Update Data Table
    var table = document.getElementById("dataTable");
    var row = table.insertRow(1);
    var cell1 = row.insertCell(0);
    var cell2 = row.insertCell(1);
    var cell3 = row.insertCell(2);
    var cell4 = row.insertCell(3);
    var cell5 = row.insertCell(4);
    var cell6 = row.insertCell(5);
    var cell7 = row.insertCell(6);
    var cell8 = row.insertCell(7);
    var cell9 = row.insertCell(8);
    cell1.innerHTML = time;
    cell3.innerHTML = obj.Temperature;
    cell4.innerHTML = obj.Ax;
    cell5.innerHTML = obj.Ay;
    cell6.innerHTML = obj.Az;
    cell7.innerHTML = obj.Gx;
    cell8.innerHTML = obj.Gy;
    cell9.innerHTML = obj.Gz;

    }
  };
  xhttp.open("GET", "readADC", true);
  xhttp.send();
}

</script>
</body>

</html>

)=====";
