# TwitterAPIAccsessor
#
#
# TwitterAPIのメソッドを叩くと同時にカウントを行う

module TweetLib
	class TwitterAPIAccessor

		attr_reader :counter
		
		def initialize(proc)
			@proc = proc
			@counter = 0
		end

		def call(args={})
			@counter = @counter + 1
			return @proc.call(args)
		end

	end
end
