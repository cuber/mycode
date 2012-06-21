<?php
global $_SC;
$_SC['a'] = 'b';
function test_global() {
    var_dump($_SC);
}
test_global();
