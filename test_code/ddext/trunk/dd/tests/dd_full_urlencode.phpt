--TEST--
Check for dd_full_urlencode
--SKIPIF--
<?php if (!extension_loaded("dd")) print "skip"; ?>
--FILE--
<?php 
$a = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()_+<>?[]\{}|~`\xE9\x46";
var_dump(dd_full_urlencode($a));
?>
--EXPECT--
string(261) "%61%62%63%64%65%66%67%68%69%6A%6B%6C%6D%6E%6F%70%71%72%73%74%75%76%77%78%79%7A%30%31%32%33%34%35%36%37%38%39%41%42%43%44%45%46%47%48%49%4A%4B%4C%4D%4E%4F%50%51%52%53%54%55%56%57%58%59%5A%21%40%23%24%25%5E%26%2A%28%29%5F%2B%3C%3E%3F%5B%5D%5C%7B%7D%7C%7E%60%E9%46"
