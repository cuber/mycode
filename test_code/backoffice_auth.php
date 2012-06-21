<?php
class Dat_Priviledge {
	private static $api_url = 'http://api.backoffice.dangdang.com/PermissionValidate.asmx/NewHasPermissionForOtherDomain?';

	public static function check($courent_url){
		//登录权限的判断
		$userCookie     = $_COOKIE['userCookies'];
		//$userCookie = 'zhongligang|True|True||27,128';
		if (empty($userCookie)) {
			//若未登陆，则跳转到登陆页
			$url = "http://backoffice.dangdang.com/wordflow_SysManager/Login.aspx";
			echo "<script>window.open ('".$url."') </script>" ;exit;
		}
		list($username, $logintype, $is_validate, $shopcode, $roleids) = explode('|', $userCookie);

		$logintype    = strtolower($logintype) === 'true' ? TRUE : FALSE;
		$is_validate  = strtolower($is_validate) === 'true' ? TRUE : FALSE;
		$roleids = explode(',', $roleids);
		$roleids_arr = array();
		foreach ($roleids AS $v) {
			$tmp_int = (int) trim($v);
			if ($tmp_int > 0) {
				$roleids_arr[] = $tmp_int;
			}
		}
		$roleids = $roleids_arr;

		if (!self::check_priviledge($roleids,$courent_url)) {
			$url = "http://backoffice.dangdang.com/wordflow_SysManager/Login.aspx";
			echo "<script>window.open ('".$url."') </script>" ;exit;
		}
	}


	private static function check_priviledge($roleids,$courent_url) {
		$post = array(
			'domainAndRaw_url' =>  $courent_url,
			'userRoles'        => 0
		);

		foreach ($roleids AS $roleid) {
			$post['userRoles'] = $roleid;
			$parameter = http_build_query($post);
			$ch = curl_init(self::$api_url.$parameter);
			curl_setopt($ch, CURLOPT_HEADER, FALSE);
			curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
			curl_setopt($ch, CURLOPT_TIMEOUT,15);
			$response = curl_exec($ch);
			$errno = curl_errno($ch);
			$errorstr = curl_error($ch);
			curl_close($ch);
			if($errno != 0)
			{
				continue;
			}
			preg_match('/\<string.*\>(.*)\<\/string\>/', $response, $match);
			if (isset($match[1])) {
				$response = explode(',', trim($match[1], '{}'));
				if (count($response) > 0) {
					for ($i = 0, $j = count($response); $i < $j; $i++) {
						$arr = explode(':', $response[$i]);
						if (count($arr) === 2
							&& trim($arr[0], '\'"') === 'authorizeLevel'
							&& trim($arr[1], '\'"') === '2') {
								return TRUE;
							}
					}
				}
			}
		}
		return FALSE;
	}


}


?>
