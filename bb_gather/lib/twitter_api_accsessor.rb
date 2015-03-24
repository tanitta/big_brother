# TwitterAPIAccsessor
#
#
# TwitterAPIのメソッドを叩くと同時にカウントを行う

module TweetLib
	class TwitterAPIAccessor
		
		def initialize(proc)
			@proc = proc
			@counter = 0
		end

		def call(args={})
			@proc.call(args)
			@counter = @counter + 1
		end

		def get_counter
			@counter
		end
	end
end
