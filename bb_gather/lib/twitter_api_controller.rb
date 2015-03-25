#
# TwitterAPIController
#

# TwitterAPIをいい感じに叩くためのコントローラ
# メソッド名をシンボルで与えて回数をカウント
# 適当に参照して上限に達したらsleepさせる感じ

module TweetLib
	class TwitterAPIController

		def initialize
			@requested_api_nums = {}
		end

		def count_api_requests(method_name)
			if @requested_api_nums[method_name.to_sym] then
				@requested_api_nums.store(method_name.to_sym, 1)
			else
				@requested_api_nums[method_name.to_sym] += 1
			end
		end

		def check_request_count(method_name)
			@requested_api_nums[method_name.to_sym].to_i
		end
		
	end
end