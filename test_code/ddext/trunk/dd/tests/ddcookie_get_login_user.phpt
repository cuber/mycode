--TEST--
Check DDCookie
--SKIPIF--
<?php if (!extension_loaded("dd")) print "skip"; ?>
--FILE--
<?php 
$dangdang_com = "email=Yml0LmtldmluQGdtYWlsLmNvbQ==&nickname=Yml0X2tldmlu&display_id=5864973647162&customerid=pmLGBw4lJ83AEOheUdwx0Q==&viptype=IUI0giRAGrg=&show_name=bit_kevin";
$login_dangdang_com =  ".ASPXAUTH=pqOx/DgWHo1QgrEKpkspEySAf9pBYZQm";

/* 两种风格无性能差异，推荐使用OO风格 */
// OO风格
$dd_cookie = new DDCookie();
$user = $dd_cookie->getLoginUser($login_dangdang_com, $dangdang_com);
var_dump($user);

// 函数风格
$dd_cookie = ddcookie_new();
$user = ddcookie_get_login_user($dd_cookie, $login_dangdang_com, $dangdang_com);
var_dump($user);

?>
--EXPECT--
array(5) {
  ["email"]=>
  string(19) "bit.kevin@gmail.com"
  ["customerid"]=>
  int(13672699)
  ["nickname"]=>
  string(9) "bit_kevin"
  ["display_id"]=>
  string(13) "5864973647162"
  ["show_name"]=>
  string(9) "bit_kevin"
}
array(5) {
  ["email"]=>
  string(19) "bit.kevin@gmail.com"
  ["customerid"]=>
  int(13672699)
  ["nickname"]=>
  string(9) "bit_kevin"
  ["display_id"]=>
  string(13) "5864973647162"
  ["show_name"]=>
  string(9) "bit_kevin"
}
