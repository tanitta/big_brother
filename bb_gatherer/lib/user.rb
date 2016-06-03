require 'uri'
require 'net/https'
require 'nokogiri'

module Gatherer
  class User
    def initialize(user_name, options = {search_limit:10})
      @name = user_name
      scrape_user(options[:search_limit])
    end

    attr_reader :name, :relations

    private
    def scrape_user(search_limit)
      html = Gatherer::html_from_uri("http://twilog.org/#{ @name }/friends")
      
      # wip
      exist_user = false
      if exist_user
        pages = 1
      end
    end

    def scrape_user_page(search_limit, page_number)
      html = Gatherer::html_from_uri("http://twilog.org/#{ @name }/friends/r-#{ page_number }")
    end
  end

  def html_from_uri(uri_string)
    uri = URI.parse(uri_string)
    http = Net::HTTP.new(uri.host, uri.port)
    if uri.scheme == 'https'
      http.use_ssl = true
      http.verify_mode = OpenSSL::SSL::VERIFY_PEER
    end
    request = Net::HTTP::Get.new(uri.request_uri)
    response = http.request(request)
    return Nokogiri::HTML(response.body)
  end
  module_function :html_from_uri
end
