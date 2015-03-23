# -*- coding : utf-8 -*-

require "rubygems"
require "nokogiri"
require "twitter"
require "open-uri"

OpenSSL::SSL::VERIFY_PEER = OpenSSL::SSL::VERIFY_NONE

class NameGather

	MAX_FETCH_ID_NUM = 15000

	def initialize
		@client_ = Twitter::REST::Client.new do |cnf|
			cnf.consumer_key = "SIZWv4zZgP9usXEJ307dcHthK"
			cnf.consumer_secret = "vpuGNmJWOoA8hdBI5qut8tjaigI6LTOgyUSnzMW91vxu8LSgPa"
			cnf.access_token = "580785994-VLjwYpd4qabAQLRhVfeBxVLydtfyfkPWsvXpHIM6"
			cnf.access_token_secret = "IINFWvMVPrTWNiDo5ba6M4RllEokgTakjNnUJ1FRY7QT0"
		end
		@screen_name_ = @client_.screen_name
	end

	# フォロワーのアカウントのIDを取得する
	# 5000[id/API]なので割りと効率は良さそう
	# MAX_FETCH_ID_NUMで設定した件数を超えそうなら取得を中止する
	def gather_num_id
		follower_num = @client_.followers_count
		id_list = []
		request_num = 1
		request_num += (follower_num % 5000) if follower_num >= 5000
		request_num.times do |n|
			break if n*5000 >= MAX_FETCH_ID_NUM
			@client_.follower_ids(@screen_name_).each do |user_id_num|
				id_list << user_id_num.to_s
			end
		end	
	end 

	# 取得したIDをスクリーンネームに変換する
	# 怒られそうな手法なのでsleep(1)を挟む
	# def gather_screen_name
	# 	screen_name_list = []
	# 	self.gather_num_id.each do |user_id|
	# 		url = "https://twitter.com/intent/user?user_id=#{user_id}"
	# 		screen_name_list << /screen_name=(.*)/.match(Nokogiri::HTML(open(url).read).css("meta[name=native-url]").first.attribute("content").to_s).to_a[1]
	# 		sleep(1)
	# 	end
	# end

end
