require 'uri'
require 'net/https'
require 'open-uri'
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
      uri = "http://twilog.org/#{ @name }/"
      html = Gatherer::html_from_uri(uri)
      
      # wip
      exist_user = html.xpath('//title').text != "Twilog"
      if exist_user
        pages = 1
      end
    end

    def scrape_user_page(search_limit, page_number)
      html = Gatherer::html_from_uri("http://twilog.org/#{ @name }/friends/r-#{ page_number }")
    end
  end

  private
  def html_from_uri(uri_string)
    # uri = URI.parse(uri_string)
    # http = Net::HTTP.new(uri.host, uri.port)
    # if uri.scheme == 'https'
    #   http.use_ssl = true
    #   http.verify_mode = OpenSSL::SSL::VERIFY_PEER
    # end
    # request = Net::HTTP::Get.new(uri.request_uri)
    # response = http.request(request)
    # p response.body
    # return Nokogiri::HTML(response.body)
    charset = nil
    html = open(uri_string, 'User-Agent'=>'firefox') do |f|
      charset = f.charset
      f.read
    end
    Nokogiri::HTML.parse(html, nil, charset)
  end
  
  module_function :html_from_uri
end
