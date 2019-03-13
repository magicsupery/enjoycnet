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

		class ServerImplInterface;
		class ClientImplInterface;
		class Proto
		{
			public:
				Proto() = default;
				virtual ~Proto() = default;

				int type() const;
				int protocol() const;
				int family() const {return family_;}

				virtual std::shared_ptr<ServerImplInterface> create_server_impl() 
				{ 
					return std::shared_ptr<ServerImplInterface>(); 
				};
				virtual std::shared_ptr<ClientImplInterface> create_client_impl() 
				{
				   	return std::shared_ptr<ClientImplInterface>();
			   	};

				static Proto* instance()
				{
					static Proto p;
					return &p;
				}

			protected:
				explicit Proto(int family, ProtoType proto_type = ProtoType::tcp)
					:family_(family),
					proto_type_(proto_type){}

				int family_;
				ProtoType proto_type_;
		};
	};
};

