<html>
<head>
<style type="text/css">
table.imagetable {
	font-family: verdana,arial,sans-serif;
	font-size:11px;
	color:#333333;
	border-width: 1px;
	border-color: #999999;
	border-collapse: collapse;
}
table.imagetable th {
	background:#b5cfd2 url('cell-blue.jpg');
	border-width: 1px;
	padding: 8px;
	border-style: solid;
	border-color: #999999;
	text-align: center;
}
table.imagetable td {
	background:#dcddc0 url('cell-grey.jpg');
	border-width: 1px;
	padding: 8px;
	border-style: solid;
	border-color: #999999;
	text-align: center;
}

#background {
    width: 100%; 
    height: 100%; 
    position: absolute; 
    left: 0px; 
    top: 0px; 
    z-index: 0;
	opacity: 0.2;
}

.stretch {
    width:100%;
    height:100%;
}

</style>
</head>
<body bgcolor="#999">


<?php

mysql_connect("localhost", "root", "0000") or die(mysql_error());

mysql_select_db("allflowlift") or de(mysql_error());

$result = mysql_query("SELECT * FROM pompen") 
or die(mysql_error());  

echo "<table class=\"imagetable\">";
echo "<tr> <th>id</th><th>Last Errors</th> <th>Looptijd pomp 1</th> <th>Looptijd pomp 2</th> <th>Aantal inschakelingen pomp 1</th> <th>Aantal inschakelingen pomp 2</th> <th>Bovenste inschakelnivo</th> <th>Onderste inschakelnivo</th> <th>Uitschakelnivo</th> <th>Nadraaitijd (Seconden)</th> <th>Nadraaitijd hoogwater (Seconden)</th> <th>Maximale looptijd pomp (min)</th> </tr>";

// Iterate through resultset
while($row = mysql_fetch_array( $result )) {
	echo "<tr><td>"; 

	echo $row['id'];
	echo "</td><td>"; 
	echo $row['last_error_1'];
	echo ", ";
	echo $row['last_error_2'];
	echo ", ";
	echo $row['last_error_3'];
	echo "</td><td>"; 
	echo $row['z_looptijd_pomp1'];
	echo "</td><td>"; 
	echo $row['z_looptijd_pomp2'];
	echo "</td><td>"; 
	echo $row['z_inschakelingen_pomp1'];
	echo "</td><td>"; 
	echo $row['z_inschakelingen_pomp2'];
	echo "</td><td>"; 
	echo $row['c_nivo_bovenste'];
	echo "</td><td>"; 
	echo $row['c_nivo_onderste'];
	echo "</td><td>"; 
	echo $row['c_nivo_uitschakel'];
	echo "</td><td>"; 
	echo $row['c_nadraai'];
	echo "</td><td>"; 
	echo $row['c_nadraai_hoogwater'];
	echo "</td><td>"; 
	echo $row['c_looptijd'];

	echo "</td></tr>"; 
} 

echo "</table>";
?>
<div id="background">
    <img src="allflowlift.jpg" class="stretch" alt="" />
</div>

</body>
</html>
