<?php

	mysql_connect("localhost","root","");
	mysql_select_db("car_db");
	$qry = "select * from cars";
	$raw = mysql_query($qry);
	while ($res= mysql_fetch_array($raw)) {
		$data[] = $res;
	}
	print(json_encode($data));
?>