#pragma once
#include <data_manager.hpp>
#include <user.hpp>
#include <pharticle/pharticle.hpp>

namespace big_brother {
	class Controller {
		public:
			int nearest_id_;
			std::set<int> selected_ids_;
			double nearest_distance_;
			double selectable_range_;
			ofEasyCam& cam_ref_;
			
			big_brother::DataManager& data_manager_ref_;
			pharticle::Engine& ph_engine_ref_;
			int diffuse_id_;
			
			Controller(big_brother::DataManager& data_manager_ref, pharticle::Engine& ph_engine_ref,ofEasyCam& cam_ref):
				ph_engine_ref_(ph_engine_ref),
				data_manager_ref_(data_manager_ref),
				cam_ref_(cam_ref),
				selectable_range_(25){};
			virtual ~Controller(){};
			
			void set_nearest_user(int id,double distance){
				nearest_id_ = id;
				nearest_distance_ = distance;
			}
			// void set_nearest_distance(distance){
			// 	nearest_distance_ = distance;
			// }
			
			void change_select(){
				if(selected_ids_.find(nearest_id_) == selected_ids_.end()){
					//見つからなかった場合
					selected_ids_.insert(nearest_id_);
				}else{
					//見つかった場合
					selected_ids_.erase(nearest_id_);
				}
			}
			void setup(){
				diffuse_id_ = data_manager_ref_.entry_id_;
			};
			
			void update(){
				diffuse_from_id();
			};
			
			void draw(){
					ofFill();
					ofSetColor(ofColor::magenta);
					ofSetLineWidth(2);
				// std::cout << "----------------" << std::endl;
				for(auto&& id : selected_ids_){
				// std::cout << "selected_id : " << id << std::endl;
					double x = data_manager_ref_.users_[id].particle_.position_[0];
					double y = data_manager_ref_.users_[id].particle_.position_[1];
					double z = data_manager_ref_.users_[id].particle_.position_[2];
					ofVec3f cur = cam_ref_.worldToScreen(ofVec3f(x,y,z));
					ofCircle(cur, 4);
				}
				
				for(auto&& user : data_manager_ref_.users_){
					double x = user.second.particle_.position_[0];
					double y = user.second.particle_.position_[1];
					double z = user.second.particle_.position_[2];
					ofVec3f cur = cam_ref_.worldToScreen(ofVec3f(x,y,z));
					if( user.second.particle_.b_static_ ){
						ofLine(cur-ofVec2f(8,0),cur+ofVec2f(8,0));
						ofLine(cur-ofVec2f(0,8),cur+ofVec2f(0,8));
					}
					
					if(user.second.id_ == diffuse_id_){
						
						ofNoFill();
						ofCircle(cur, 8);
					}
				}
				// if(nearest_distance_ < selectable_range_){
				// 	ofSetColor(ofColor::gray);
				// 	ofLine(nearest_vertex, mouse);
				//
				// 	ofNoFill();
				// 	ofSetColor(ofColor::magenta);
				// 	ofSetLineWidth(2);
				// 	ofCircle(nearest_vertex, 4);
				// 	ofSetLineWidth(1);
				//
				// 	ofVec2f offset(10, -10);
				// 	std::string info = "";
				// 	info +="user_id : "+ofToString(nearest_id);
				// 	info +="\n";
				// 	info +="screen_name : "+ofToString(data_manager_.users_[nearest_id].screen_name_);
				// 	ofDrawBitmapStringHighlight(info, mouse + offset);
				// }
			}
			
			void set_diffuse_id(int id){
				diffuse_id_ = id;
			}
			
			void key_pressed(std::string str){
				if(str == "d"){
					for(auto&& i : selected_ids_){
						set_diffuse_id(i);
					}
					selected_ids_.clear();
				}
				if(str == "l"){
					for(auto&& i :selected_ids_){
						toggle_lock(i);
					}
					selected_ids_.clear();
				}
				if(str == "m"){
					if(selected_ids_.size()>0){
						Eigen::Vector3d pos;(0,0,0);

						for(auto&& i :selected_ids_){
							// toggle_lock(i);
							pos += data_manager_ref_.users_[i].particle_.position_;
						}
						pos = pos / selected_ids_.size();
						for(auto&& user : data_manager_ref_.users_){
							user.second.particle_.position_ -= pos;
						}

						selected_ids_.clear();
					}
				}
			}
			void toggle_lock(int id){
				data_manager_ref_.users_[id].particle_.b_static_ = !data_manager_ref_.users_[id].particle_.b_static_;
			}
			
			void diffuse_from_id(){
				int id = diffuse_id_;
				auto func_diffusion = [=](pharticle::Particle& p1, pharticle::Particle& p2){
					Eigen::Vector3d v(0,0,0);
					v = (p2.position_ - p1.position_);
					// v.normalize();
					Eigen::Vector3d f(0,0,0);
					if(v.norm() != 0){
						v.normalize();
						f = v*5;
					}
					// Eigen::Vector3d nv = v.normalized();
					// v = 0;
					// v +=(p1.velocity_ - p2.velocity_)*0.5;
					// v +=(- p2.velocity_)*0.2;
					return f;
				};
				for(auto&& user : data_manager_ref_.users_){
					ph_engine_ref_.add_constraint_pair(&data_manager_ref_.users_[id].particle_, &user.second.particle_,func_diffusion);
				}
			}
	};
} // namespace big_brother
