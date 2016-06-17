import armos;
import pharticle;

class User{
    public{
        this(in string name, in ar.Vector3d position = ar.Vector3d(0, 0, 0)){
            _name = name;
            _growth = 0.0;
            _entity = new pharticle.Particle();
            _entity.mass = 1.0;

            _radius = 30.0;
            _entity.radius= 0.0;

            import std.conv;
            _entity.position = position;
            _entity.velocity = ar.Vector3d(0, 0, 0);
            _entity.acceleration = ar.Vector3d(0, 0, 0);
        }

        string name()const{
            return _name;
        }

        float radius()const{
            return _radius;
        }

        void update(){
            if (_growth < 1.0){
                _growth += 0.005;
            }
            import easing;
            _entity.radius = _growth.map!easeOutElastic(0.0, 1.0, 1.0, _radius);
        }

        ar.Vector3d position()const{
            return _entity.position;
        }

        pharticle.Particle entity(){
            return _entity;
        }

        void draw(ar.BitmapFont font){
            ar.pushMatrix;
            ar.translate(position);
            ar.setColor(64);
            ar.circlePrimitive(0.0, 0.0, 0.0, _entity.radius*0.75).drawFill;
            ar.setColor(200);
            font.draw(name, 0, 0);
            ar.popMatrix;
        }
    }

    private{
        string _name;
        float _growth;
        float _radius;
        pharticle.Particle _entity;
    }
}

unittest{
    assert(__traits(compiles, { auto user = new User("hoge"); }));
}

class Relation{
    this(User from, User to, in int weight){
        this.from = from;
        this.to = to;
        this.weight = weight;

        import std.conv;
        void delegate(ref pharticle.Particle, ref pharticle.Particle) func = (ref pharticle.Particle p1, ref pharticle.Particle p2){
            ar.Vector3d d = p2.position - p1.position;
            p2.addForce( d.normalized*(500.0/this.weight.to!float - d.norm)*20.0 );
        };

        _pair = pharticle.ConstraintPair(from.entity, to.entity, func);
    }

    void draw(){
        ar.setColor(64);
        ar.drawLine(from.entity.position, to.entity.position);
    }

    User from;
    User to;

    ref pharticle.ConstraintPair pair(){ return _pair; };

    private{
        int weight;
        pharticle.ConstraintPair _pair;
    }
}

unittest{
    auto userFrom = new User("from");
    auto userTo= new User("to");
    auto weight = 3;
    auto relation = new Relation(userFrom, userTo, weight); 

    assert(relation.from == userFrom);
    assert(relation.to == userTo);
    assert(relation.weight == weight);
}

class Gatherer{
    void detect(User from, int limit){
        import std.stdio;
        import std.process;
        import std.string;
        import std.algorithm;
        import std.conv;

        auto output = execute(["ruby", "../bb_gatherer/bin/gather", from.name, limit.text]).output;
        if(!output.empty){
            auto listWithoutEmpty = output.split("\n")[0..$-1];
            foreach (str; listWithoutEmpty) {
                import std.random;
                ar.Vector3d spawnPoint = from.position + from.radius.to!double * ar.Vector3d(uniform(-1.0, 1.0), uniform(-1.0, 1.0), 0.0).normalized;
                User to = new User(str.split(":")[0], spawnPoint);
                int weight = str.split(":")[1].to!int;

                _users[to.name] = to;
                _relations[from.name][to.name] = weight;
                _users_slice ~= to;
                // _relations_slice ~= relation;
            }
        }
    }

    User[string] users(){
        return _users;
    }

    User[] users_slice(){
        return _users_slice;
    }

    int[string][string] relations(){
        return _relations;
    }

    Relation[] relations_slice(){
        return _relations_slice;
    }

    void clear(){
        _users.clear;
        _users_slice = [];
        _relations.clear;
        _relations_slice= [];
    }

    private{
        User[string] _users;
        User[] _users_slice;
        int[string][string] _relations;
        Relation[] _relations_slice;
    }
}


import core.thread;
class TestApp : ar.BaseApp{
    this(){
        _engine = new pharticle.Engine;
        _engine.unitTime = 1.0/30.0;
        _engine.setReactionForceFunction = (ref pharticle.Particle p1, ref pharticle.Particle p2){
            ar.Vector3d d = p2.position - p1.position;
            double d_length = d.norm;
            double depth = d_length - ( p1.radius + p2.radius );
            if(depth < 0){
                ar.Vector3d vab = p2.velocity - p1.velocity;
                double cd = 50;
                double e = 0.0;
                double c = (p1.mass * p2.mass)/(p1.mass+p2.mass) * ( (1.0+e) * vab.dotProduct(d.normalized) + cd * depth );
                p2.addForce( -d.normalized*c);
                p2.addForce(-vab*0.005);
            }
        };
    }

    override void setup(){
        ar.blendMode(ar.BlendMode.Alpha);

        _font = new ar.BitmapFont();
        _font.load("font.png", 8, 8);

        string targetUserName = "ttata_trit";
        _users[targetUserName] = new User(targetUserName);
        _targetUser = _users[targetUserName];

        _threadGather = new Thread({gather;}).start;
    }


    override void update(){
        foreach (ref user; _users) {
            user.update;
            user.entity.addForce(-user.entity.velocity*1.0);
            _engine.add(user.entity);
        }

        foreach (string userFromName,  ref tos; _relations) {
            foreach (string  userToName,  ref relation; tos) {
                import std.stdio;
                _engine.add(relation.pair);
            }
        }

        _engine.update;
    }

    override void draw(){
        ar.pushMatrix;
        import std.conv;
        ar.translate(ar.windowSize[0].to!float*0.5, ar.windowSize[1].to!float*0.5, 0.0);
        foreach (string userFromName,  ref tos; _relations) {
            foreach (string userToName,  ref relation; tos) {
                relation.draw;
            }
        }

        foreach (ref user; _users) {
            user.draw(_font);
        }

        ar.popMatrix;
    }

    override void keyPressed(int key){}

    override void keyReleased(int key){}

    override void mouseMoved(ar.Vector2i position, int button){}

    override void mousePressed(ar.Vector2i position, int button){}

    override void mouseReleased(ar.Vector2i position, int button){}
    
    override void exit(){
        _shouldExit = true;
        _threadGather.join;
    }

    private{
        User _targetUser;

        User[string] _users;
        Relation[string][string] _relations;
        double _counter;

        ar.BitmapFont _font;

        pharticle.Engine _engine;
        
        bool _shouldExit = false;
        Thread _threadGather;
        
        void gather(){
            while(!_shouldExit){
                Gatherer gatherer = new Gatherer;

                import std.stdio;
                gatherer.detect(_targetUser, 10);

                import std.algorithm.searching;
                foreach (ref user; gatherer.users) {
                    if(!_users.keys.canFind(user.name)){
                        _users[user.name] = user;
                    }
                }

                import std.algorithm;
                foreach (string userFromName,  ref tos; gatherer.relations) {
                    foreach (string userToName,  ref relation; tos) {
                        if(!_relations.keys.canFind(userFromName) || !_relations[userFromName].keys.canFind(userToName)){
                            _relations[userFromName][userToName] = new Relation(_users[userFromName], _users[userToName], relation );
                        }
                    }
                }

                {
                    bool isNewUser = false;
                    int nextUserIndex = 0;
                    do{
                        nextUserIndex.writeln;
                        _targetUser = gatherer.users_slice[nextUserIndex];
                        isNewUser = !_relations.keys.canFind(_targetUser.name);
                        nextUserIndex++;
                    }while(!isNewUser);
                }

                "nextTarget".writeln;
                _targetUser.name.writeln;

                gatherer.clear;
            }
        }
    }
}

void main(){ar.run(new TestApp);}
