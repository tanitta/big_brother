#pragma once

#include <ofMain.h>
#include <pharticle/pharticle.hpp>
#include <data_manager.hpp>
#include <user.hpp>
#include <controller.hpp>
namespace big_brother {
	class BBAnalyzer : public ofBaseApp{
		private:
			pharticle::Engine ph_engine_;
			DataManager data_manager_;
			Controller controller_;
			
			int nearest_id_;
			double nearest_distance_;
			ofVec2f nearest_vertex_;
			
			ofEasyCam cam_;
		public:
			BBAnalyzer():controller_(data_manager_,ph_engine_,cam_),
		nearest_id_(-1),nearest_distance_(0),nearest_vertex_(){};
			virtual ~BBAnalyzer(){};
			void setup(){
				//graphics
				ofEnableAlphaBlending();
				ofEnableDepthTest();
				ofEnableAntiAliasing();
				ofBackground(32,32,32);
				
				data_manager_.load_from_xml("hoge.xml");
				data_manager_.setup_particle();
				controller_.setup();
				
				ph_engine_.set_func_reaction_force([](pharticle::Particle& p1, pharticle::Particle& p2){
					Eigen::Vector3d f(0,0,0);
					Eigen::Vector3d p(0,0,0);
					p = (p2.position_ - p1.position_);
					
					double r = p1.radius_ + p2.radius_;
					Eigen::Vector3d v_r = r*p.normalized();
					f = (v_r - p)*0.5;
					f +=(p1.velocity_ - p2.velocity_)*0.1;
					// p = (p2.position_ - p1.position_)*p2.mass_*0.01*v.norm();
					return f;
				});
				
				auto func_spring = [=](pharticle::Particle& p1, pharticle::Particle& p2){
					Eigen::Vector3d v(0,0,0);
					v = (p2.position_ - p1.position_);
					Eigen::Vector3d nv = v.normalized();
					v = (nv*(120) - v)*0.5;
					v += (p1.velocity_ - p2.velocity_)*0.5;
					v +=(- p2.velocity_)*0.5;
					return v;
				};
				
				for (auto&& user : data_manager_.users_) {
					ph_engine_.add_particle_ptr(user.second.particle_);
				}
				
				data_manager_.generate_relation(func_spring);
				
				data_manager_.print();
				std::cout << data_manager_.relations_.size() << std::endl;
				
				data_manager_.randomize(-200,200);
				
				data_manager_.users_[data_manager_.entry_id_].particle_.b_static_ = true;
				data_manager_.users_[data_manager_.entry_id_].particle_.position_[0] = 0;
				data_manager_.users_[data_manager_.entry_id_].particle_.position_[1] = 0;
				data_manager_.users_[data_manager_.entry_id_].particle_.position_[2] = 0;
			};
			
			void update(){
				controller_.update();
				
				for (auto&& pair : data_manager_.relations_) {
					ph_engine_.add_constraint_pair_bdi(pair);
				}
				ph_engine_.update();
				
				
				
				// int n = mesh.getNumVertices();
				// float nearestDistance = 0;
				// ofVec2f nearestVertex;
				// int nearestIndex = 0;
				// ofVec2f mouse(mouseX, mouseY);
				// for(int i = 0; i < n; i++) {
				// 	ofVec3f cur = cam.worldToScreen(mesh.getVertex(i));
				// 	float distance = cur.distance(mouse);
				// 	if(i == 0 || distance < nearestDistance) {
				// 		nearestDistance = distance;
				// 		nearestVertex = cur;
				// 		nearestIndex = i;
				// 	}
				// }
				nearest_distance_ = 0;
				nearest_vertex_;
				nearest_id_ = -1;
				ofVec2f mouse(mouseX, mouseY);
				for(auto&& user : data_manager_.users_){
					double x = user.second.particle_.position_[0];
					double y = user.second.particle_.position_[1];
					double z = user.second.particle_.position_[2];
					ofVec3f cur = cam_.worldToScreen(ofVec3f(x,y,z));
					// ofSetColor(ofColor::white);
					// ofFill();
					// ofCircle(cur, 2);
					double distance = cur.distance(mouse);
					if(nearest_id_ == -1 || distance < nearest_distance_) {
						nearest_distance_ = distance;
						nearest_vertex_ = cur;
						nearest_id_ = user.second.id_;
					}
				}
				controller_.set_nearest_user(nearest_id_,nearest_distance_);
			};
			
			void draw(){
				cam_.begin();
				ofFill();
				ofSetColor(255,255,255);
				for (auto&& user : data_manager_.users_) {
					double x = user.second.particle_.position_[0];
					double y = user.second.particle_.position_[1];
					double z = user.second.particle_.position_[2];
					ofSphere(x,y,z,10);
					
				}
				
				for (auto&& pair : data_manager_.relations_) {
					double x1 = pair.particle_ptrs_[0]->position_[0];
					double y1 = pair.particle_ptrs_[0]->position_[1];
					double z1 = pair.particle_ptrs_[0]->position_[2];
					
					double x2 = pair.particle_ptrs_[1]->position_[0];
					double y2 = pair.particle_ptrs_[1]->position_[1];
					double z2 = pair.particle_ptrs_[1]->position_[2];
					
					ofLine(x1,y1,z1,x2,y2,z2);
				}
				cam_.end();
				
				 
				// std::cout << "nearest_distance: " << nearest_distance<< std::endl;
				// 
				
				if(nearest_distance_ < 25){
					ofVec2f mouse(mouseX, mouseY);
					ofSetColor(ofColor::gray);
					ofLine(nearest_vertex_, mouse);
				
					ofNoFill();
					ofSetColor(ofColor::magenta);
					ofSetLineWidth(2);
					ofCircle(nearest_vertex_, 4);
					ofSetLineWidth(1);
				
					ofVec2f offset(10, -10);
					std::string info = "";
					info +="user_id : "+ofToString(nearest_id_);
					info +="\n";
					info +="screen_name : "+ofToString(data_manager_.users_[nearest_id_].screen_name_);
					ofDrawBitmapStringHighlight(info, mouse + offset);
				}
				
				controller_.draw();
			};
			
			void keyPressed(int key){
				char keys[] = {(char)key};
				std::string strKeys=(std::string)keys;
				strKeys.resize(1);
				controller_.key_pressed(strKeys);
			};
			void keyReleased(int key){};
			void mouseMoved(int x, int y ){};
			void mouseDragged(int x, int y, int button){
			};
			void mousePressed(int x, int y, int button){
				if(nearest_distance_ < 25){
					controller_.change_select();
				}
			};
			void mouseReleased(int x, int y, int button){};
			void windowResized(int w, int h){};
			void dragEvent(ofDragInfo dragInfo){};
			void gotMessage(ofMessage msg){};
	};
} // namespace big_brother
