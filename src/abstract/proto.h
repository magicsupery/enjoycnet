#pragma once
#include <boost/asio.hpp>
namespace enjoyc
{
	namespace net
	{
		enum class ProtoType 
		{
			unkown,
			tcp,
			udp,
			http,
		};

		class ServerBase;
		class ClientBase;
		class Proto
		{
			public:
				virtual ~Proto() {}

				int type() const;
				int protocol() const;
				int family() const {return family_;}

				virtual std::shared_ptr<ServerBase> create_server_impl() { return std::shared_ptr<ServerBase>(); };
				virtual std::shared_ptr<ClientBase> create_client_impl() { return std::shared_ptr<ClientBase>(); };
			protected:
				explicit Proto(int family, ProtoType proto_type = ProtoType::tcp)
					:family_(family),
					proto_type_(proto_type){}

				int family_;
				ProtoType proto_type_;
		};
	};
};

