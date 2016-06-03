require 'uri'
require 'net/https'
require 'nokogiri'

class User
  def initialize(user_name, options = {search_limit:10})
    @name = user_name
  end
  
  attr_reader :name, :followers
end
