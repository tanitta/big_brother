# API Limit cheking program

require 'rubygems'
require 'twitter'

OpenSSL::SSL::VERIFY_PEER = OpenSSL::SSL::VERIFY_NONE

client = Twitter::REST::Client.new do |cnf|
	cnf.consumer_key = "SIZWv4zZgP9usXEJ307dcHthK"
	cnf.consumer_secret = "vpuGNmJWOoA8hdBI5qut8tjaigI6LTOgyUSnzMW91vxu8LSgPa"
	cnf.access_token = "580785994-VLjwYpd4qabAQLRhVfeBxVLydtfyfkPWsvXpHIM6"
	cnf.access_token_secret = "IINFWvMVPrTWNiDo5ba6M4RllEokgTakjNnUJ1FRY7QT0"
end

follower_list = client.followers("toshiemon18")

api_count = 0
start_time = Time::now
begin
	follower_list.each do |user|
		user.followers(user.screen_name.to_s)
		# puts user.screen_name.to_s
		sleep(3)
		api_count += 1
	end
rescue
	sleep(15 * 60 + 10)
	retry
	# puts "API Count : #{api_count}"
	# puts " Rate limit exceeded."
	# exit
end
end_time = Time::now
puts "やっと終わったぜ"
puts "走査時間:#{end_time - start_time}"

#  ---- メモ ---
# 鍵垢は走査対象から排除