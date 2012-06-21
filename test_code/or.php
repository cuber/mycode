<?php

a() || b();

function a() {
	var_dump("a");
	return true;
}

function b() {
	var_dump("b");
	return true;
}
