const char indexHTML[] PROGMEM = R"=====(



<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script src="https://code.highcharts.com/highcharts.js"></script>
  <style>
    h1 {
      font-family: Arial;
      text-align: center;
    }
    #chart {
      max-width: 800px;
      margin: 0 auto;
    }
    #buttons {
      width: 100%;
      text-align: center;
    }
    button {
      border: none;
      background-color: #0af;
      padding: 10px 14px;
      margin: 5px 2px;
      width: 200px;
      font-size: 20px;
      cursor: pointer;
    }
    button:hover {
      background-color: #09e;
    }
  </style>
</head>
<body>
  <h1>MPU6050 Vibration Detector</h1>
  <div id="chart"></div>
  <div id="buttons">
    <input type="hidden" id="mode" name="mode">
    <button onClick="location.replace('/measuremode')">Measure mode</button>
    <button onClick="location.replace('/blinkmode')">Blink mode</button>
  </div>
</body>
<script>
  var chart = new Highcharts.Chart({
    chart: { renderTo: "chart" },
    title: { text: "Chart" },
    series: [{
      showInLegend: false,
      data: []
    }],
    plotOptions: {
      line: {
        dataLabels: { enabled: false },
        marker: { enabled: false }
      },
      series: { color: "#000" }
    },
    yAxis: {
      title: { text: "Power" }
    },
    credits: { enabled: false }
  });
  setInterval(() => {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if(this.readyState == 4 && this.status == 200) {
        var x = "";
        var y = parseFloat(this.responseText);
        if(chart.series[0].data.length > 150) {
          chart.series[0].addPoint([x, y], true, true, true);
        }
        else {
          chart.series[0].addPoint([x, y], true, false, true);
        }
      }
    };
    xhttp.open("GET", "/power", true);
    xhttp.send();
  }, 150);
</script>
</html>



)=====";
