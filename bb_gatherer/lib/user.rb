require 'uri'
require 'net/https'
require 'open-uri'
require 'nokogiri'

module Gatherer
  class User
    def initialize(user_name, options = {search_limit:10})
      @name = user_name
      @relations = {}
      scrape_user(options[:search_limit])
    end

    attr_reader :name, :relations

    private
    def scrape_user(search_users_limit)
      uri = "http://twilog.org/#{ @name }/"
      html = Gatherer::html_from_uri(uri)
      
      exist_user = html.xpath('//title').text != "Twilog"
      if exist_user
        existing_pages = user_friend_pages
        search_pages = (search_users_limit.to_f/100.0).ceil
        
        [existing_pages, search_pages].min.times do |p|
          if p == search_pages-1
            scrape_user_page(search_users_limit - 100*p, p)
          else
            scrape_user_page(100, p)
          end
        end
      end
    end
    
    def user_friend_pages
      uri = "http://twilog.org/#{ @name }/friends"
      html = Gatherer::html_from_uri(uri)
      pagenation = html.search('ul[class="pagination"]')[0]
      pagenation ? pagenation.search('li')[pagenation.search('li').length - 2].children[0].text.to_i : 1
    end

    def scrape_user_page(search_limit, page_number)
      html = Gatherer::html_from_uri("http://twilog.org/#{ @name }/friends/r-#{ page_number+1 }")
      
      relation_tags = html.search('ul[class="main-list"]')[0].search('li')
      relation_tags[0..search_limit-1].each do |tag| 
        name, weight = relation_from_tag(tag) 
        @relations[name] = weight
      end
    end
    
    def relation_from_tag(li_tag)
      name = ""
      weight = 0
      is_valid_user = li_tag.search('a[class="side-list-icon"]').size != 0
      if is_valid_user 
        name = li_tag.search('a')[1].children[0].text.delete('@')
        weight = li_tag.search('a')[1].children[1].children.text.to_i
      else
        name = li_tag.search('a')[0].children[0].text.delete('@')
        weight = li_tag.search('a')[0].children[1].text.to_i
      end
      
      return name, weight
    end
  end

  private
  def html_from_uri(uri_string)
    charset = nil
    html = open(uri_string, 'User-Agent'=>'firefox') do |f|
      charset = f.charset
      f.read
    end
    Nokogiri::HTML.parse(html, nil, charset)
  end
  
  module_function :html_from_uri
end
