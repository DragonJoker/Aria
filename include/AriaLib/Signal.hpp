/*
See LICENSE file in root folder
*/
#ifndef ___Aria_Signal_HPP___
#define ___Aria_Signal_HPP___

#include "Prerequisites.hpp"

#include "AriaLib/BeginExternHeaderGuard.hpp"
#include <functional>
#include <map>
#include <set>
#include "AriaLib/EndExternHeaderGuard.hpp"

namespace aria
{
	template< typename SignalT >
	class Connection
	{
	private:
		using my_signal = SignalT;
		using my_signal_ptr = my_signal *;
		Connection( Connection< my_signal > const & ) = delete;
		Connection & operator=( Connection< my_signal > const & ) = delete;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		Connection()
			: m_connection{ 0u }
			, m_signal{ nullptr }
		{
		}
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	connection	The connection to the signal.
		 *\param[in]	signal		The signal.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	connection	La connexion au signal.
		 *\param[in]	signal		Le signal.
		 */
		Connection( uint32_t connection, my_signal & signal )
			: m_connection{ connection }
			, m_signal{ &signal }
		{
			signal.addConnection( *this );
		}
		/**
		 *\~english
		 *\brief			Move constructor.
		 *\param[in,out]	rhs	The object to move.
		 *\~french
		 *\brief			Constructeur par déplacement.
		 *\param[in,out]	rhs	L'objet à déplacer.
		 */
		Connection( Connection< my_signal > && rhs )noexcept
			: m_connection{ rhs.m_connection }
			, m_signal{ rhs.m_signal }
		{
			if ( m_signal )
			{
				m_signal->replaceConnection( rhs, *this );
			}

			rhs.m_signal = nullptr;
			rhs.m_connection = 0u;
		}
		/**
		 *\~english
		 *\brief			Move assignment operator.
		 *\param[in,out]	rhs	The object to move.
		 *\~french
		 *\brief			Opérateur d'affectation par déplacement.
		 *\param[in,out]	rhs	L'objet à déplacer.
		 */
		Connection & operator=( Connection< my_signal > && rhs )noexcept
		{
			if ( &rhs != this )
			{
				disconnect();
				m_connection = rhs.m_connection;
				m_signal = rhs.m_signal;

				if ( m_signal )
				{
					m_signal->replaceConnection( rhs, *this );
				}

				rhs.m_signal = nullptr;
				rhs.m_connection = 0u;
			}

			return *this;
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\remarks		Disconnects the function from the signal.
		 *\~french
		 *\brief		Destructeur.
		 *\remarks		Déconnecte la fonction du signal.
		 */
		~Connection()
		{
			disconnect();
		}
		/**
		 *\~english
		 *\brief		Disconnects the function from the signal.
		 *\~french
		 *\brief		Déconnecte la fonction du signal.
		 */
		bool disconnect()
		{
			bool result{ false };

			if ( m_signal && m_connection )
			{
				m_signal->disconnect( m_connection );
				m_signal->removeConnection( *this );
				m_signal = nullptr;
				m_connection = 0u;
				result = true;
			}

			return result;
		}

	private:
		/**
		 *\~english
		 *\brief		Swaps two connections.
		 *\~french
		 *\brief		Echange deux connexions.
		 */
		void swap( Connection & lhs, Connection & rhs )
		{
			if ( &rhs != &lhs )
			{
				std::swap( lhs.m_signal, rhs.m_signal );
				std::swap( lhs.m_connection, rhs.m_connection );
			}
		}

	private:
		//!\~english	The connection identifier.
		//!\~french		L'identifiant de la connection.
		uint32_t m_connection;
		//!\~english	The signal.
		//!\~french		Le signal.
		my_signal_ptr m_signal;
	};

	template< typename Function >
	class Signal
	{
		friend class Connection< Signal< Function > >;
		using my_connection = Connection< Signal< Function > >;
		using my_connection_ptr = my_connection *;

	public:
		using connection = my_connection;

	public:
		/**
		 *\~english
		 *\brief		Destructor.
		 *\remarks		Disconnects all remaining connections.
		 *\~french
		 *\brief		Destructeur.
		 *\remarks		Déconnecte toutes les connexions restantes.
		 */
		~Signal()
		{
			// Connection::disconnect appelle Signal::remConnection, qui
			// supprime la connection de m_connections, invalidant ainsi
			// l'itérateur, donc on ne peut pas utiliser un for_each, ni
			// un range for loop.
			auto it = m_connections.begin();

			while ( it != m_connections.end() )
			{
				auto disco = ( *it )->disconnect();
				CU_Require( disco );
				it = m_connections.begin();
			}
		}
		/**
		 *\~english
		 *\brief		Connects a new function that will be called if the signal is emitted.
		 *\param[in]	function	The function.
		 *\return		The function index, in order to be able to disconnect it.
		 *\~french
		 *\brief		Connecte une nouvelle fonction, qui sera appelée lorsque le signal est émis.
		 *\param[in]	function	La fonction.
		 *\return		L'indice de la fonction, afin de pouvoir la déconnecter.
		 */
		my_connection connect( Function function )
		{
			uint32_t index = 1u;

			if ( !m_slots.empty() )
			{
				index = m_slots.rbegin()->first + 1u;
			}

			m_slots.emplace( index, function );
			return my_connection{ index, *this };
		}
		/**
		 *\~english
		 *\brief		Emits the signal, calls every connected function.
		 *\~french
		 *\brief		Emet le signal, appelant toutes les fonctions connectées.
		 */
		void operator()()const
		{
			for ( auto it : m_slots )
			{
				it.second();
			}
		}
		/**
		 *\~english
		 *\brief		Emits the signal, calls every connected function.
		 *\param[in]	params	The functions parameters.
		 *\~french
		 *\brief		Emet le signal, appelant toutes les fonctions connectées.
		 *\param[in]	params	Les paramètres des fonctions.
		 */
		template< typename ... Params >
		void operator()( Params && ... params )const
		{
			for ( auto it : m_slots )
			{
				it.second( std::forward< Params >( params )... );
			}
		}

	private:
		/**
		 *\~english
		 *\brief		Disconnects a function.
		 *\param[in]	index	The function index.
		 *\~french
		 *\brief		Déconnecte une fonction.
		 *\param[in]	index	L'indice de la fonction.
		 */
		void disconnect( uint32_t index )
		{
			auto it = m_slots.find( index );

			if ( it != m_slots.end() )
			{
				m_slots.erase( it );
			}
		}
		/**
		 *\~english
		 *\brief		adds a connection to the list.
		 *\param[in]	connection	The connection to add.
		 *\~french
		 *\brief		Ajoute une connexion à la liste.
		 *\param[in]	connection	La connexion à ajouter.
		 */
		void addConnection( my_connection & connection )
		{
			assert( m_connections.find( &connection ) == m_connections.end() );
			m_connections.insert( &connection );
		}
		/**
		 *\~english
		 *\brief		Removes a connection from the list.
		 *\param[in]	connection	The connection to remove.
		 *\~french
		 *\brief		Enlève une connexion de la liste.
		 *\param[in]	connection	La connexion à enlever.
		 */
		void removeConnection( my_connection & connection )
		{
			assert( m_connections.find( &connection ) != m_connections.end() );
			m_connections.erase( &connection );
		}
		/**
		 *\~english
		 *\brief		Removes a connection from the list.
		 *\param[in]	oldConnection	The connection to remove.
		 *\param[in]	newConnection	The connection to add.
		 *\~french
		 *\brief		Enlève une connexion de la liste.
		 *\param[in]	oldConnection	La connexion à enlever.
		 *\param[in]	newConnection	La connexion à ajouter.
		 */
		void replaceConnection( my_connection & oldConnection
			, my_connection & newConnection )
		{
			removeConnection( oldConnection );
			addConnection( newConnection );
		}

	private:
		//!\~english	The connected functions list.
		//!\~french		La liste des fonctions connectées.
		std::map< uint32_t, Function > m_slots;
		//!\~english	The connections list.
		//!\~french		La liste des connections à ce signal.
		std::set< my_connection_ptr > m_connections;
	};
}

#endif
