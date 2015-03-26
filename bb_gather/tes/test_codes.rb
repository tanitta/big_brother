# TweetLib::ClientのGETの処理の共通化をするためのメソッド案
def oauth_get_status(end_point, args={})
	@access_token.get(end_point.to_s, args)
end