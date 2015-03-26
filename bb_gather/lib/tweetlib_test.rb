require "./tweetlib"

include TweetLib

key_array = []
key_array << "SIZWv4zZgP9usXEJ307dcHthK"
key_array << "vpuGNmJWOoA8hdBI5qut8tjaigI6LTOgyUSnzMW91vxu8LSgPa"
key_array << "580785994-VLjwYpd4qabAQLRhVfeBxVLydtfyfkPWsvXpHIM6"
key_array << "IINFWvMVPrTWNiDo5ba6M4RllEokgTakjNnUJ1FRY7QT0"

client = Client.new(key_array)
# folloers_idsの動作テスト
# puts client.followers_ids(screen_name: "toshiemon18", count: 5000)

# show_userの動作テスト
puts client.show_users(screen_name: "toshiemon18")
