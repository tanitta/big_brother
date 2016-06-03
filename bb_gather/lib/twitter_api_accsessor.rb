# TwitterAPIAccsessor
#
#
# TwitterAPIのメソッドを叩くと同時にカウントを行う

module TweetLib
	class TwitterAPIAccessor

		attr_reader :counter
		attr_accessor :rate_limit
		
		def initialize(proc, rate_limit)
			@proc = proc
			@counter = 0
			@rate_limit = rate_limit
		end

		def call(args={})
			@counter = @counter + 1
			@counter = 0 if @counter >= @rate_limit
			@proc.call(args)
		end

	end
end
