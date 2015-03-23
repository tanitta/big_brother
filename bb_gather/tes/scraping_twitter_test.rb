# -*- coding : utf-8 -*-

require "rubygems"
require "nokogiri"
require "twitter"
require "open-uri"


OpenSSL::SSL::VERIFY_PEER = OpenSSL::SSL::VERIFY_NONE

client = Twitter::REST::Client.new do |cnf|
	cnf.consumer_key = "SIZWv4zZgP9usXEJ307dcHthK"
	cnf.consumer_secret = "vpuGNmJWOoA8hdBI5qut8tjaigI6LTOgyUSnzMW91vxu8LSgPa"
	cnf.access_token = "580785994-VLjwYpd4qabAQLRhVfeBxVLydtfyfkPWsvXpHIM6"
	cnf.access_token_secret = "IINFWvMVPrTWNiDo5ba6M4RllEokgTakjNnUJ1FRY7QT0"
end

follower_list = []
follower_list = client.follower_ids("toshiemon18").to_a
# url = "https://twitter.com/intent/user?user_id=#{follower_list[0].to_s}"
# puts /screen_name=(.*)/.match(Nokogiri::HTML(open(url).read).css("meta[name=native-url]").first.attribute("content").to_s).to_a[1]
puts JSON.parse(
		Net::HTTP.get(
			URI.parse("https://twitter.com/users/show.json?user_id=#{follower_list[0].to_s}")
		)
	)
